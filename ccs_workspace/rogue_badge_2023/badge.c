/// mini.badge.lgbt 2023 primary high-level event-driven driver.
/**
 ** The badge source module controls the high-level behavior of the badge
 ** application.
 **
 ** The main control loop is based in the lower-level main.c module, which
 ** calls the functions in badge.c from the appropriate context.
 **
 ** The basic split in responsibility between the badge.c and main.c modules
 ** is that main.c detects, prioritizes, and clears flags set from
 ** interrupts; it then calls the appropriate function in badge.c so that
 ** badge.c can behave in a more event-driven way, with the underlying MSP430
 ** hardware and registers abstracted away by main.c for the most part.
 **
 ** \file badge.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2023 George Louthan @duplico. MIT License.
 */

#include <stdint.h>
#include <stdlib.h>

#include <msp430fr2633.h>

#include "badge.h"
#include "rtc.h"

#pragma PERSISTENT(badge_conf)
/// The main persistent badge configuration.
volatile badge_conf_t badge_conf = (badge_conf_t){
    .badge_id = BADGE_ID_UNASSIGNED,
    .badges_seen = {0,},
    .badges_seen_count = 1, // I've seen myself.
};

/// Whether this badge thinks it has an authoritative clock.
uint8_t badge_clock_authority = 0; // Intentionally clears on power cycle.

/// Set the current time in our persistent config.
inline void badge_set_time(uint32_t clock, uint8_t authority) {
    rtc_seconds = clock;
    badge_clock_authority = authority;

    fram_unlock();
    badge_conf.clock = clock;
    fram_lock();
}

/// Callback for a long button press.
void badge_button_press_long() {
    // TODO

    button_state = 1; // Allow repeated long presses
}

/// Callback for a short button press.
void badge_button_press_short() {
    // TODO
}

/// Initialize the badge application behavior.
void badge_init() {
    // TODO
}
