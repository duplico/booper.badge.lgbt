#include <msp430.h> 
/// mini.badge.lgbt 2023 low-level entry point.
/**
 ** The main source module is concerned with the basic setup of the MCU and
 ** onboard GPIO and peripherals. It also contains the main program loop of
 ** the entire badge code. Aside from initialization of peripherals and the
 ** other badge drivers, this module has a core purpose of calling events in
 ** (primarily) the badge.c module.
 **
 ** The basic split in responsibility between the badge.c and main.c modules
 ** is that main.c detects, prioritizes, and clears flags set from
 ** interrupts; it then calls the appropriate function in badge.c so that
 ** badge.c can behave in a more event-driven way, with the underlying MSP430
 ** hardware and registers abstracted away by main.c for the most part.
 **
 ** \file main.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2023 George Louthan @duplico. MIT License.
 */

// MSP430 headers
#include <msp430fr2633.h>
#include <driverlib.h>

// System headers
#include <stdint.h>

// CapTIvate
#include "captivate.h"
#include "CAPT_App.h"

// Project headers
#include <badge.h>
#include "rtc.h"
#include "tlc5948a.h"

/// Current button state (1 for pressed, 2 for long-pressed, 0 not pressed).
volatile uint8_t button_state;

/// Interrupt flag for the system clock tick.
volatile uint8_t f_time_loop;
/// Interrupt flag for the button being held for over 1 second.
volatile uint8_t f_button_press_long;
/// Interrupt flag that ticks every second.
volatile uint8_t f_second;

/// Perform the TI-recommended software trim of the DCO per TI demo code.
void dco_software_trim()
{
    unsigned int oldDcoTap = 0xffff;
    unsigned int newDcoTap = 0xffff;
    unsigned int newDcoDelta = 0xffff;
    unsigned int bestDcoDelta = 0xffff;
    unsigned int csCtl0Copy = 0;
    unsigned int csCtl1Copy = 0;
    unsigned int csCtl0Read = 0;
    unsigned int csCtl1Read = 0;
    unsigned int dcoFreqTrim = 3;
    unsigned char endLoop = 0;

    do
    {
        CSCTL0 = 0x100;                         // DCO Tap = 256
        do
        {
            CSCTL7 &= ~DCOFFG;                  // Clear DCO fault flag
        }while (CSCTL7 & DCOFFG);               // Test DCO fault flag

        __delay_cycles((unsigned int)3000 * MCLK_FREQ_MHZ);// Wait FLL lock status (FLLUNLOCK) to be stable
                                                           // Suggest to wait 24 cycles of divided FLL reference clock
        while((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));

        csCtl0Read = CSCTL0;                   // Read CSCTL0
        csCtl1Read = CSCTL1;                   // Read CSCTL1

        oldDcoTap = newDcoTap;                 // Record DCOTAP value of last time
        newDcoTap = csCtl0Read & 0x01ff;       // Get DCOTAP value of this time
        dcoFreqTrim = (csCtl1Read & 0x0070)>>4;// Get DCOFTRIM value

        if(newDcoTap < 256)                    // DCOTAP < 256
        {
            newDcoDelta = 256 - newDcoTap;     // Delta value between DCPTAP and 256
            if((oldDcoTap != 0xffff) && (oldDcoTap >= 256)) // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim--;
                CSCTL1 = (csCtl1Read & (~(DCOFTRIM0+DCOFTRIM1+DCOFTRIM2))) | (dcoFreqTrim<<4);
            }
        }
        else                                   // DCOTAP >= 256
        {
            newDcoDelta = newDcoTap - 256;     // Delta value between DCPTAP and 256
            if(oldDcoTap < 256)                // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim++;
                CSCTL1 = (csCtl1Read & (~(DCOFTRIM0+DCOFTRIM1+DCOFTRIM2))) | (dcoFreqTrim<<4);
            }
        }

        if(newDcoDelta < bestDcoDelta)         // Record DCOTAP closest to 256
        {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }

    }while(endLoop == 0);                      // Poll until endLoop == 1

    CSCTL0 = csCtl0Copy;                       // Reload locked DCOTAP
    CSCTL1 = csCtl1Copy;                       // Reload locked DCOFTRIM
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)); // Poll until FLL is locked
}

/// Prepare to write to FRAM by disabling interrupts and unlocking write access to INFOA.
inline volatile void fram_unlock() {
    __bic_SR_register(GIE);
    SYSCFG0 = FRWPPW | PFWP;
}

/// Finish writing to FRAM by locking write access to INFOA and enabling interrupts.
inline volatile void fram_lock() {
    SYSCFG0 = FRWPPW | DFWP | PFWP;
    __bis_SR_register(GIE);
}

/// Initialize clock signals and the three system clocks.
/**
 ** We'll take the DCO to 8 MHz, and divide it by 1 for MCLK = 8MHz.
 ** Then we'll divide MCLK by 1 to get 8 MHz SMCLK.
 **
 ** Our available clock sources are:
 **  VLO:     10kHz very low power low-freq
 **  REFO:    32.768kHz (typ) reference oscillator
 **  DCO:     Digitally controlled oscillator (1MHz default)
 **           Specifically, 1048576 Hz typical.
 **
 ** At startup, our clocks are as follows:
 **  MCLK:  Sourced by the DCO
 **         (Available: DCO, REFO, VLO)
 **  SMCLK: Sourced from MCLK, with no divider
 **         (Available dividers: {1,2,4,8})
 **  ACLK: Sourced from REFO
 **         (the only available internal source)
 **
 ** So the only change we need to make is to the DCO and MCLK.
 **
 */
void init_clocks() {

    __bis_SR_register(SCG0); // disable FLL
    CSCTL3 |= SELREF__REFOCLK; // Set REFO as FLL reference source
    CSCTL1 = DCOFTRIMEN | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_3; // DCOFTRIM=3, DCO Range = 8MHz
    CSCTL2 = FLLD_0 + 243; // DCODIV = 8MHz
    __delay_cycles(3); // Documentation says to wait at least 3 cycles
    __bic_SR_register(SCG0); // enable FLL
    dco_software_trim(); // Software Trim to get the best DCOFTRIM value

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz

    // default DCODIV as MCLK and SMCLK source; no need to modify CSCTL5.
}

/// Apply the initial configuration of the GPIO and peripheral pins.
/**
 **
 */
void init_io() {
    // Per datasheet S4.6, p20, unused pins should be switched to outputs.

    // IO:
    // P1.0     UCB0 CSN    (SEL 00; DIR 1) // TODO: GPIO or UCB0STE?
    // P1.1     UCB0CLK     (SEL 10; DIR 1) // TODO
    // P1.2     UCB0SIMO    (SEL 00; DIR 1) // TODO
    // P1.3     UCB0SOMI    (SEL 00; DIR 1) // TODO
    // P1.4        (SEL 01; DIR 1)
    // P1.5        (SEL 01; DIR 0)
    // P1.6        (SEL 01; DIR 1)
    // P1.7     SMCLK out   (SEL 00; DIR 1) // TODO

    // P2.0     Unused      (SEL 00; DIR 1)
    // P2.1     Unused      (SEL 00; DIR 1)
    // P2.2     Unused      (SEL 00; DIR 1)
    // P2.3     Unused      (SEL 00; DIR 1)
    // P2.4     UCA1CLK     (SEL 00; DIR 0; REN 1; OUT 1) // TODO
    // P2.5     UCA1SOMI    (SEL 01; DIR 0) // TODO
    // P2.6     UCA1SIMO    (SEL 01; DIR 1) // TODO
    // P2.7     Unused      (SEL 00; DIR 1)

    // P3.0 is DONTCARE for CAP0.0
    // P3.1     Unused      (SEL 00; DIR 1)
    // P3.2     Unused      (SEL 00; DIR 1)

    // P1
    P1DIR =     0b11011111;
    P1SEL0 =    0b01110000; // LSB
    P1SEL1 =    0b00000010; // MSB
    P1REN =     0x00;
    P1OUT =     0x00;

    // P2
    P2DIR =     0b11001111;
    P2SEL0 =    0b01100000; // LSB
    P2SEL1 =    0b00000000; // MSB
    P2REN =     0b00010000;
    P2OUT =     0b00010000;

    // Init P3 as unused
    P3DIR = 0xFF;
    P3SEL0 = 0x00;
    P3SEL1 = 0x00;
    P3OUT = 0x00;

    // Unlock the pins from high-impedance mode:
    // (AKA the MSP430FR magic make-it-work command)
    PM5CTL0 &= ~LOCKLPM5;
}

/// Callback from CapTIvate for a change in the button state.
void button_cb(tSensor *pSensor) {

    if((pSensor->bSensorTouch == true) && (pSensor->bSensorPrevTouch == false))
    {
        // Button press
         button_state = 1;
         rtc_button_csecs = 0;
    }

    if((pSensor->bSensorTouch == false) && (pSensor->bSensorPrevTouch == true))
    {
        // Button release
         if (button_state == 1) {
             // Only fire if it's not being long-pressed.
             badge_button_press_short();
         }
         button_state = 0;
    }
}

/// Make snafucated
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Board basics initialization
	init_clocks();

	// Mid-level drivers initialization
	rtc_init();
	tlc_init();

	// CapTIvate initialization and startup
    MAP_CAPT_initUI(&g_uiApp);
    MAP_CAPT_calibrateUI(&g_uiApp);
    MAP_CAPT_registerCallback(&B1, &button_cb);

    MAP_CAPT_stopTimer();
    MAP_CAPT_clearTimer();
    MAP_CAPT_selectTimerSource(CAPT_TIMER_SRC_ACLK);
    MAP_CAPT_selectTimerSourceDivider(CAPT_TIMER_CLKDIV__1);
    MAP_CAPT_writeTimerCompRegister(CAPT_MS_TO_CYCLES(g_uiApp.ui16ActiveModeScanPeriod));
    MAP_CAPT_startTimer();
    MAP_CAPT_enableISR(CAPT_TIMER_INTERRUPT);

	// Application functionality initialization

    // Set up WDT, and we're off to see the wizard.
    WDTCTL = WDTPW | WDTSSEL__ACLK | WDTIS__32K | WDTCNTCL; // 1 second WDT

	while (1) {
	    // The 100 Hz loop
	    if (f_time_loop) {
            f_time_loop = 0;

	        // pat pat pat
	        WDTCTL = WDTPW | WDTSSEL__ACLK | WDTIS__32K | WDTCNTCL; // 1 second WDT
	    }

	    // The 1 Hz loop
	    if (f_second) {
            f_second = 0;

	        if (!(rtc_seconds % BADGE_CLOCK_WRITE_INTERVAL)) {
	            // Every BADGE_CLOCK_WRITE_INTERVAL seconds, write our time to the config.
	            rtc_set_time(rtc_seconds, rtc_clock_authority);
	        }
	    }

	    if (f_button_press_long) {
            f_button_press_long = 0;

	        badge_button_press_long();
	    }

	    // Check whether CapTIvate needs to be serviced.
	    if (g_bConvTimerFlag) {
            g_bConvTimerFlag = 0;

	        CAPT_updateUI(&g_uiApp);
	    }

	    // Enter sleep mode if we have no unserviced flags.
	    if (
	            !f_time_loop &&
	            !f_second &&
	            !f_button_press_long &&
	            !g_bConvTimerFlag
	    ) {
	        __bis_SR_register(LPM0_bits); // TODO: Select LPM to use
	    }
	}
}
