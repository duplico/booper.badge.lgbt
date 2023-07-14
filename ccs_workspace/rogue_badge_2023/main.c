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

// MSP430 main header
#include <msp430fr2633.h>

// C headers
#include <stdint.h>

// Accessory headers
#include <driverlib.h>

// Project headers
#include <badge.h>

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

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Board basics
	init_clocks();

	// Mid-level drivers

	// Application functionality

	return 0;
}
