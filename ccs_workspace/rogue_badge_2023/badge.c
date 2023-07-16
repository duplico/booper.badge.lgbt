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
