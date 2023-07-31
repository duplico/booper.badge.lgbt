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
#include "util.h"

#pragma PERSISTENT(badge_conf)
/// The main persistent badge configuration.
volatile badge_conf_t badge_conf = (badge_conf_t){
    .badge_id = BADGE_ID_UNASSIGNED,
    .badges_seen = {0,},
    .badges_seen_count = 1, // I've seen myself.
};

/// Mark a badge as seen, returning 1 if it's a new badge or 2 if a new uber.
void badge_set_seen(uint8_t id) {
    static uint8_t last_badge_seen = 255;
    static uint32_t last_badge_seen_time = 0;

    if (id >= 8*BADGES_SEEN_BUFFER_LEN_BYTES) {
        return; // Invalid ID.
    }

    // TODO: Necessary?
    if (
            last_badge_seen_time && // If we've set the last seen time...
//            rtc_seconds < last_badge_seen_time + BADGE_PAIR_COOLDOWN  && // And it's before our cooldown period...
            id == last_badge_seen // And we're seeing the same badge
    ) {
        return; // Ignore it.
    }

    last_badge_seen = id;
    last_badge_seen_time = rtc_seconds;

    uint8_t seen = check_id_buf(id, (uint8_t *) badge_conf.badges_seen);

    if (seen) {
        return;
    }

    // New badge!
    fram_unlock();

    set_id_buf(id, (uint8_t *) (badge_conf.badges_seen));

    if (badge_conf.badges_seen_count < UINT8_MAX) {
        badge_conf.badges_seen_count++;
    }

    fram_lock();

    // TODO:
    // uint8_t new_anim_id = LEDS_ID_NO_ANIM;
//    if (is_uber(id)) {
//    } else {
//    }
//    if (new_anim_id != LEDS_ID_NO_ANIM) {
//        leds_start_anim_by_id(new_anim_id, 0, 1, 0);
//    }

    // TODO: Needed?
    // Queue our connection count.
//    leds_start_anim_by_id(ANIM_META_CONNECTS, 0, 0, 0);
}

/// Set badge ID in the configuration.
void badge_set_id(uint8_t id) {
    uint8_t old_id = badge_conf.badge_id;

    if (id != badge_conf.badge_id) {
        fram_unlock();
        badge_conf.badge_id = id;
        unset_id_buf(old_id, badge_conf.badges_seen);
        set_id_buf(badge_conf.badge_id, badge_conf.badges_seen);
        // TODO:
//        if (!is_uber(old_id) && is_uber(id)) {
//            badge_conf.ubers_seen_count++;
//        } else if (is_uber(old_id) && !is_uber(id)) {
//            badge_conf.ubers_seen_count--;
//        }
        fram_lock();

        // Re-seed PRNG.
        srand(badge_conf.badge_id);
    }
}

// TODO:
/// Returns the number of lights that should be lit to represent the current badge_seen_count.
//uint8_t badge_count_lights() {
//    if (badge_conf.badges_seen_count >= BADGES_SEEN_MAX_DISP) {
//        return 15;
//    }
//
//    return 1 + badge_conf.badges_seen_count/BADGES_SEEN_PER_DISP;
//}

/// Callback for a long button press.
void badge_button_press_long() {
    button_state = 1; // Allow repeated long presses
}

/// Callback for a short button press.
void badge_button_press_short() {
}

/// Initialize the badge application behavior.
void badge_init() {
}
