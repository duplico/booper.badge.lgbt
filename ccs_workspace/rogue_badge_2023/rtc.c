/// Real-time clock configuration and events module.
/**
 ** This module operates a real-time clock, sourced from the 8-MHz MCLK.
 ** It works about medium well, precision-wise. But that's fine, because it
 ** only has to last a weekend! (unofficial #badgelife motto).
 **
 ** Basically, the RTC generates the main system tick, which is every 10 ms,
 ** or 100 times per second. That centisecond (csec) system tick is then
 ** used to create another, once per second tick, used to keep track of time.
 ** The system seconds timer is calibrated to measure the seconds since noon
 ** on Wednesday, Las Vegas time. Therefore, here are some real example times:
 **
 ** Seconds  |  Real time
 ** ------:  |  :--------
 ** 0        |  Noon Wednesday
 ** 43200    |  Midnight Thursday morning
 ** 86400    |  Noon Thursday
 ** 129600   |  Midnight Friday morning
 ** 172800   |  Noon Friday
 ** 212400   |  11pm Friday (party!)
 ** 216000   |  Midnight Saturday morning
 ** 302400   |  Midnight Monday morning
 **
 ** \file rtc.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2022-23 George Louthan @duplico. MIT License.
 */

#include <stdint.h>

#include <msp430fr2633.h>

#include "badge.h"

/// The number of system ticks the button has been held down so far.
volatile uint16_t rtc_button_csecs = 0;
/// System ticks this, which wraps from 99 to 0.
volatile uint8_t rtc_centiseconds = 0;
/// Number of seconds so far; persisted in `badge_conf.clock`.
volatile uint32_t rtc_seconds = 0;

/// Whether this badge thinks it has an authoritative clock.
uint8_t rtc_clock_authority = 0; // Intentionally clears on power cycle.

/// Set the current time in our persistent config.
inline void rtc_set_time(uint32_t clock, uint8_t authority) {
    rtc_seconds = clock;
    rtc_clock_authority = authority;

    fram_unlock();
    badge_conf.clock = clock;
    fram_lock();
}

/// Initialize the on-board real-time clock to tick 100 times per second.
/**
 ** This sources the RTC from SMCLK (8 MHz) divided by 1000 (8 kHz),
 ** setting the modulo to 80, so that the RTC will tick 100x
 ** per second.
 */
void rtc_init() {
//    rtc_seconds = badge_conf.clock; // TODO

    RTCMOD = 80; // Count the clock to 80 before resetting.

    // Read and then throw away RTCIV to clear the interrupt.
    volatile uint16_t vector_read;
    vector_read = RTCIV;

    RTCCTL = RTCSS__SMCLK |     // SMCLK (8MHz) source
             RTCPS__1000 |      // divided by 1000 to get 8kHz
             RTCSR |            // Reset counter.
             RTCIE;             // Enable interrupt.
}

/// RTC overflow interrupt service routine.
#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void) {
    // Called when the RTC overflows (100 times per second)
    if (RTCIV == RTCIV_RTCIF) {
        // Tick our 100 Hz time loop, first.
        rtc_centiseconds++;
        f_time_loop = 1;

        // Handle long pressing of buttons.
        if (button_state == 1) { // Not already long pressed
            rtc_button_csecs++;
            if (rtc_button_csecs == BUTTON_LONG_PRESS_CSECS) {
                f_button_press_long = 1;
                button_state = 2;
                rtc_button_csecs = 0;
            }
        }

        // Tick our 1 Hz time loop if needed.
        if (rtc_centiseconds == 100) {
            rtc_seconds++;
            f_second = 1;
            rtc_centiseconds = 0;
        }

        // Exit LPM.
        LPM0_EXIT; // TODO: Select appropriate LPM
    }
}
