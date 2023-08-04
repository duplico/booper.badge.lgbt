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
#include "leds.h"

uint8_t badge_boop_radio_cooldown = 0;

#pragma PERSISTENT(badge_conf)
/// The main persistent badge configuration.
volatile badge_conf_t badge_conf = (badge_conf_t){
    .badge_id = BADGE_ID_UNASSIGNED,
    .badges_seen = {0,},
    .badges_seen_count = 1, // I've seen myself.
};

void badge_update_queerdar_count(uint8_t badges_nearby) {
    if (badges_nearby > 20)
        leds_scan_speed = 8;
    else if (badges_nearby > 16)
        leds_scan_speed = 16;
    else if (badges_nearby > 12)
        leds_scan_speed = 24;
    else if (badges_nearby > 8)
        leds_scan_speed = 32;
    else if (badges_nearby > 6)
        leds_scan_speed = 40;
    else if (badges_nearby > 4)
        leds_scan_speed = 48;
    else if (badges_nearby > 3)
        leds_scan_speed = 56;
    else if (badges_nearby > 2)
        leds_scan_speed = 64;
    else if (badges_nearby > 1)
        leds_scan_speed = 72;
    else if (badges_nearby)
        leds_scan_speed = 80;
    else
        leds_scan_speed = 0;
}

/// Mark a badge as seen, returning 1 if it's a new badge or 2 if a new uber.
void badge_set_seen(uint8_t id) {
    if (!badge_conf.bootstrapped)
        return;

    if (id >= 8*BADGES_SEEN_BUFFER_LEN_BYTES) {
        return; // Invalid ID.
    }

    uint8_t seen = check_id_buf(id, (uint8_t *) badge_conf.badges_seen);

    if (seen) {
        leds_queerdar_alert(LEDS_QUEERDAR_OLDBADGE);
        return;
    }

    // New badge!
    fram_unlock();

    set_id_buf(id, (uint8_t *) (badge_conf.badges_seen));

    if (badge_conf.badges_seen_count < UINT8_MAX) {
        badge_conf.badges_seen_count++;
    }

    fram_lock();

    leds_queerdar_alert(LEDS_QUEERDAR_NEWBADGE);
}

/// Set badge ID in the configuration.
void badge_set_id(uint8_t id) {
    uint8_t old_id = badge_conf.badge_id;

    if (id != badge_conf.badge_id) {
        fram_unlock();

        badge_conf.badge_id = id;
        unset_id_buf(old_id, badge_conf.badges_seen);
        set_id_buf(badge_conf.badge_id, badge_conf.badges_seen);

        fram_lock();

        // Re-seed PRNG.
        srand(badge_conf.badge_id);
    }
}

/// Callback for a long button press.
void badge_button_press_long() {
    // Cycle brightness.
    leds_next_brightness();
}

/// Callback for a short button press.
void badge_button_press_short() {
    if (!badge_conf.bootstrapped) {
        fram_unlock();
        badge_conf.bootstrapped = 1;
        fram_lock();
        leds_queerdar_alert(LEDS_QUEERDAR_OLDBADGE);
        return;
    }

    leds_boop();
    if (!badge_boop_radio_cooldown) {
        badge_boop_radio_cooldown = BADGE_RADIO_BOOP_COOLDOWN;
        s_boop_radio = 1;
    }
}

/// Initialize the badge application behavior.
void badge_init() {
    // TODO: badge init
    badge_set_id(14);
}
