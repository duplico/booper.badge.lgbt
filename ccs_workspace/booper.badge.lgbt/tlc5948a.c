/// Low-level driver for TI TLC5948A PWM LED controller.
/**
 ** This driver controls the TLC5948A LED controller. Its job is simply to
 ** keep the display going. Application logic, including animations or other
 ** instructions to change the LEDs based on desired application behavior.
 ** All the animation logic lives in the leds.c module.
 **
 ** In order to use this driver, the application should directly manipulate
 ** the tlc_gs_data buffer, which holds 16 grayscale words; their mapping
 ** to physical LEDs is hardware-specific but follows the linear order of
 ** the output lines on the hardware. Once the grayscale data is placed in
 ** the buffer, call tlc_send_gs().
 **
 ** Helper functions are provided for manipulating the main parts of the
 ** function buffer, which controls device settings like display blanking,
 ** dot-correction multiplier, and global brightness correction. The
 ** helper functions only manipulate the function data buffer; to commit it
 ** to the hardware, it must be sent using tlc_send_fun().
 **
 ** \file tlc5948a.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2015-2023 George Louthan @duplico. MIT License.
 */

#include "tlc5948a.h"

#include <stdint.h>
#include <string.h>
#include <driverlib.h>

#include "badge.h"

/// Flag to the hardware module that the message is grayscale.
#define TLC_THISISGS    0x00
/// Flag to the hardware module that the message is function data.
#define TLC_THISISFUN   0x01

/// Current sending state of the SPI state machine.
uint8_t tlc_send_type = TLC_SEND_IDLE;
/// Index of the currently sending byte in the buffer.
uint8_t tlc_tx_index = 0;

/// If we are performing a loopback serial test, the data to send.
uint8_t tlc_loopback_data_out = 0x00;
/// If we are performing a loopback serial test, the data received so far.
volatile uint8_t tlc_loopback_data_in = 0x00;

/// Main buffer to hold grayscale data.
uint16_t tlc_gs_data[16] = { 0x0000, };

/// The basic set of function data, some of which can be edited.
uint8_t fun_base[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ...reserved...
        // B136 / PSM(D2)       0
        // B135 / PSM(D1)       0
        // B134 / PSM(D0)       0
        // B133 / OLDENA        0
        // B132 / IDMCUR(D1)    0
        // B131 / IDMCUR(D0)    0
        // B130 / IDMRPT(D0)    0
        // B129 / IDMENA        0
        // B128 / LATTMG(D1)    1: (byte 15)
        0x01,
        // B127 / LATTMG(D0)    1
        // B126 / LSDVLT(D1)    0
        // B125 / LSDVLT(D0)    0
        // B124 / LODVLT(D1)    0
        // B123 / LODVLT(D0)    0
        // B122 / ESPWM         1
        // B121 / TMGRST        0
        // B120 / DSPRPT        1: (byte 16)
        0b10000101,
        // B119 / BLANK
        // and 7 bits of global brightness control: (byte 17)
        TLC_BC,
        // HERE WE SWITCH TO 7-BIT SPI.
        // The following index is 18:
        TLC_DC, TLC_DC, TLC_DC, TLC_DC, // 16 7-byte DC values
        TLC_DC, TLC_DC, TLC_DC, TLC_DC,
        TLC_DC, TLC_DC, TLC_DC, TLC_DC,
        TLC_DC, TLC_DC, TLC_DC, TLC_DC
};

/// Send our current grayscale buffer to the hardware.
void tlc_set_gs() {
    while (tlc_send_type != TLC_SEND_IDLE);
    tlc_send_type = TLC_SEND_TYPE_GS;
    tlc_tx_index = 0;
    EUSCI_A_SPI_transmitData(TLC_EUSCI_BASE, TLC_THISISGS);
}

/// Send our current function data buffer to the hardware.
void tlc_set_fun() {
    while (tlc_send_type != TLC_SEND_IDLE);
    tlc_send_type = TLC_SEND_TYPE_FUN;
    tlc_tx_index = 0;
    EUSCI_A_SPI_transmitData(TLC_EUSCI_BASE, TLC_THISISFUN);
}

// TODO:
/// Stage global brightness into dot correct if different from default.
/**
 ** This is designed to give us a greater range of hardware brightness
 ** settings. The reason we would want this is that the global
 ** brightness setting and the dot-correct settings actually adjust the
 ** constant current of the LEDs. This lets our PWM grayscale action
 ** look consistent across brightness correction levels. We still
 ** retain the ability to do per-color dot correction with the
 ** preprocessor defines that start with TLC_DC.
 */
//void tlc_stage_dc_mult(uint8_t mult) {
//    for (uint8_t i=0; i<15; i+=3) {
//        fun_base[19+i + 0] = TLC_DC * mult;
//        fun_base[19+i + 1] = TLC_DC * mult;
//        fun_base[19+i + 2] = TLC_DC * mult;
//    }
//}

/// Set or unset the blank bit in the function data, but don't send it yet.
void tlc_stage_blank(uint8_t blank) {
    if (blank) {
        fun_base[17] |= BIT7;
        fun_base[16] &= ~BIT1; // TODO
    } else {
        fun_base[17] &= ~BIT7;
        fun_base[16] |= BIT1; // TODO
    }
}

/// Test the TLC chip with a shift-register loopback, returning 0 for success.
uint8_t tlc_test_loopback(uint8_t test_pattern) {
    // Send the test pattern 34 times, and expect to receive it shifted
    // a bit.
    tlc_loopback_data_out = test_pattern;
    while (tlc_send_type != TLC_SEND_IDLE);

    EUSCI_A_SPI_clearInterrupt(TLC_EUSCI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(TLC_EUSCI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);

    tlc_send_type = TLC_SEND_TYPE_LB;
    tlc_tx_index = 0;
    EUSCI_A_SPI_transmitData(TLC_EUSCI_BASE, test_pattern);
    // Spin while we send and receive:
    while (tlc_send_type != TLC_SEND_IDLE);

    EUSCI_A_SPI_disableInterrupt(TLC_EUSCI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);

    return tlc_loopback_data_in != (uint8_t) ((test_pattern << 7) | (test_pattern >> 1));
}

/// Stage global brightness setting.
void tlc_stage_bc(uint8_t bc) {
    bc = bc & 0b01111111; // Mask out BLANK just in case.
    fun_base[17] &= 0b10000000;
    fun_base[17] |= bc;
}

/// Initialize the TLC5948A hardware driver.
void tlc_init() {
    // We're assuming that the GPIO/peripheral selection has already been configured elsewhere.
    // However, we need to make sure LAT starts out low:
    LAT_POUT &= ~LAT_PBIT;

    // Configure our SPI peripheral to talk to the LED controller.
    TLC_USCI_CTLW0 |= UCSWRST;  // Shut down the eUSCI,

    // And our eUSCI peripheral:
    EUSCI_A_SPI_initMasterParam ini = {0};
    ini.clockPhase = EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT;
    ini.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW;
    ini.clockSourceFrequency = SMCLK_RATE_HZ;
    ini.desiredSpiClock = 1000000; // TODO: I reduced this
    ini.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    ini.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_SMCLK;
    ini.spiMode = EUSCI_A_SPI_3PIN;
    EUSCI_A_SPI_initMaster(TLC_EUSCI_BASE, &ini);

    TLC_USCI_CTLW0 &= ~UC7BIT;  //  put it in 8-bit mode out of caution.
    TLC_USCI_CTLW0 &= ~UCSWRST; //  and enable it.

    EUSCI_A_SPI_clearInterrupt(TLC_EUSCI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
    EUSCI_A_SPI_enableInterrupt(TLC_EUSCI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);

    // Stage an un-blank configuration to the function data:
    tlc_stage_blank(0);
//    tlc_stage_dc_mult(1); // TODO


    for (uint8_t i=0; i<16; i++) {
        tlc_gs_data[i] = 0x8888;
//        tlc_gs_data[i] = 0xffff;
    }

    // And our initial grayscale data:
    tlc_set_gs();
    // Send our initial function data:
    tlc_set_fun();
    // And our initial grayscale data: // TODO
    tlc_set_gs();
}

/// SPI interrupt service routine for the peripheral connected to the LED driver.
#pragma vector=TLC_USCI_VECTOR
__interrupt void TLC_EUSCI_ISR(void)
{
    switch (__even_in_range(TLC_USCI_IV, 4)) {
    //Vector 2 - RXIFG
    case 2:
        // We received some garbage sent to us while we were sending.
        if (tlc_send_type == TLC_SEND_TYPE_LB) {
            // We're only interested in it if we're doing a loopback test.
            tlc_loopback_data_in = EUSCI_A_SPI_receiveData(TLC_EUSCI_BASE);
            __no_operation();
        } else {
            EUSCI_A_SPI_receiveData(TLC_EUSCI_BASE); // Throw it away.
        }
        break; // End of RXIFG ///////////////////////////////////////////////////////

    case 4: // Vector 4 - TXIFG : I just sent a byte.
        if (tlc_send_type == TLC_SEND_TYPE_GS) {
            if (tlc_tx_index == 32) { // done
                LAT_POUT |= LAT_PBIT; LAT_POUT &= ~LAT_PBIT; // Pulse LAT
                tlc_send_type = TLC_SEND_IDLE;
                break;
            } else { // gs - MSB first; this starts with 0.
                volatile static uint16_t channel_gs = 0;
                channel_gs = tlc_gs_data[tlc_tx_index/2];
                __no_operation();
                if (tlc_tx_index & 0x01) { // odd; less significant byte
                    TLC_USCI_TXBUF = (channel_gs & 0xff);
                } else { // even; more significant byte
                    TLC_USCI_TXBUF = (channel_gs >> 8) & 0xff;
                }
            }
            tlc_tx_index++;
        } else if (tlc_send_type == TLC_SEND_TYPE_FUN) {
            if (tlc_tx_index == 18) { // after 18 we have to switch to 7-bit mode.
                TLC_USCI_CTLW0 |= UCSWRST;
                TLC_USCI_CTLW0 |= UC7BIT;
                TLC_USCI_CTLW0 &= ~UCSWRST;
                EUSCI_A_SPI_clearInterrupt(TLC_EUSCI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
                EUSCI_A_SPI_enableInterrupt(TLC_EUSCI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
            } else if (tlc_tx_index == 34) {
                LAT_POUT |= LAT_PBIT; LAT_POUT &= ~LAT_PBIT; // Pulse LAT
                TLC_USCI_CTLW0 |= UCSWRST;
                TLC_USCI_CTLW0 &= ~UC7BIT;
                TLC_USCI_CTLW0 &= ~UCSWRST;
                EUSCI_A_SPI_clearInterrupt(TLC_EUSCI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
                EUSCI_A_SPI_enableInterrupt(TLC_EUSCI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);
                tlc_send_type = TLC_SEND_IDLE;
                break;
            }
            EUSCI_A_SPI_transmitData(TLC_EUSCI_BASE, fun_base[tlc_tx_index]);
            tlc_tx_index++;
        } else if (tlc_send_type == TLC_SEND_TYPE_LB) { // Loopback for POST
            if (tlc_tx_index == 33) {
                tlc_send_type = TLC_SEND_IDLE;
                break;
            }
            EUSCI_A_SPI_transmitData(TLC_EUSCI_BASE, tlc_loopback_data_out);
            tlc_tx_index++;
        } else {
            tlc_send_type = TLC_SEND_IDLE; // probably shouldn't reach.
        }
        break; // End of TXIFG /////////////////////////////////////////////////////
    default: break;
    } // End of ISR flag switch ////////////////////////////////////////////////////
}
