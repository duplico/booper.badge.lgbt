/// Application-level driver for managing LEDs for booper.badge.lgbt.
/**
 ** This module is used for driving the application-specific behavior
 ** of the LEDs for the badge. For the actual hardware and peripheral
 ** interface with the TLC5948A LED driver module, see the low-level
 ** driver, tlc5948a.c and tlc5948a.h.
 **
 ** \file leds.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2023 George Louthan @duplico. MIT License.
 */

#include <stdlib.h>

#include "badge.h"
#include "leds.h"
#include "tlc5948a.h"
#include "animations.h"
#include "eyes.h"

// General configuration of the 7-segs
/// The standard brightness of the LEDs.
uint16_t leds_brightness = BADGE_BRIGHTNESS_1;

// Eye animations
/// The current display configuration of the eyes.
eye_t leds_eyes_curr[2] = {0, }; // Start all off

#pragma PERSISTENT(leds_eyes_ambient)
/// The ambient eye display to return to after an animation completes.
uint8_t leds_eyes_ambient = EYES_NORMAL;
/// The current animation, or null if there is no current animation.
eye_anim_t *eye_anim_curr = 0x0000;
/// The number of loops left to do in the current animation.
uint8_t eye_anim_curr_loops;
/// The current frame number of the current animation.
uint8_t eye_anim_curr_frame;
/// The number of LED timesteps remaining in the current animation frame.
uint8_t eye_anim_curr_ticks;
/// Whether to blink between the end of this animation and ambient eyes.
uint8_t eye_anim_blink_transition;

/// The temporary ambient eyes
eye_t leds_eyes_ambient_temp[2] = {0, };
/// How many more ticks to show the temporary ambient eyes
uint16_t leds_eyes_ambient_temp_ticks = 0;

/// Number of LED timesteps remaining in the current blink animation
uint8_t eye_blinking = 0;

// Dot-scanner animations
/// Current scan speed, where 0 is off but otherwise higher is slower. 80 = about 1 second.
uint16_t leds_scan_speed = 0; // 8..80 inclusive is a good range here.
/// The brightness of the scan dots, although this can be overridden by leds_eyes_curr.
uint16_t leds_dot_level[] = {0x0000, 0x0000};
/// Which eye's scan dot is currently on (and fading out).
uint8_t scan_dot_curr = 0;

/// Helper function to return what the current ambient eye display should be.
eye_t eye_ambient(uint8_t eye_index) {
    if (leds_eyes_ambient_temp_ticks) {
        return leds_eyes_ambient_temp[eye_index];
    }

    return EYES_DISP[leds_eyes_ambient][eye_index];
}

/// Stage the eye data into the TLC grayscale data, and send it to the LED driver.
void leds_load_gs() {
    for (uint8_t eye = 0; eye <= 1; eye++) {
        tlc_gs_data[eye*8 + 0] = leds_eyes_curr[eye].tl ? leds_brightness : 0;
        tlc_gs_data[eye*8 + 1] = leds_eyes_curr[eye].l ? leds_brightness : 0;
        tlc_gs_data[eye*8 + 2] = leds_eyes_curr[eye].bl ? leds_brightness : 0;
        tlc_gs_data[eye*8 + 3] = leds_eyes_curr[eye].m ? leds_brightness : 0;
        // Index offset 4 is the dot - handled below.
        tlc_gs_data[eye*8 + 5] = leds_eyes_curr[eye].tr ? leds_brightness : 0;
        tlc_gs_data[eye*8 + 6] = leds_eyes_curr[eye].r ? leds_brightness : 0;
        tlc_gs_data[eye*8 + 7] = leds_eyes_curr[eye].br ? leds_brightness : 0;

        // Handle the dot:
        if (leds_eyes_curr[eye].dot) {
            // If the current eye setup says it should be on, then force it on.
            tlc_gs_data[eye*8 + 4] = leds_brightness;
        } else if (leds_scan_speed) {
            // Otherwise, follow their dot level variable.
            tlc_gs_data[eye*8 + 4] = leds_dot_level[eye];
        } else {
            tlc_gs_data[eye*8 + 4] = 0x0000;
        }
    }

    tlc_set_gs();
}

/// Blink the eyes.
void do_blink() {
    eye_blinking = BLINK_TICKS;
    leds_eyes_curr[0] = EYE_OFF;
    leds_eyes_curr[1] = EYE_OFF;

    for (uint8_t eye=0; eye<2; eye++) {
        if (eye_ambient(eye).bl || eye_ambient(eye).l || eye_ambient(eye).tl) {
            leds_eyes_curr[eye].bl = 1;
        }
        if (eye_ambient(eye).br || eye_ambient(eye).r || eye_ambient(eye).tr) {
            leds_eyes_curr[eye].br = 1;
        }
    }

    leds_load_gs();
}

/// Cycle through the available brightness levels, one per function call.
void leds_next_brightness() {
    switch(leds_brightness) {
    case BADGE_BRIGHTNESS_0:
        leds_brightness = BADGE_BRIGHTNESS_1;
        break;
    case BADGE_BRIGHTNESS_1:
        leds_brightness = BADGE_BRIGHTNESS_2;
        break;
    case BADGE_BRIGHTNESS_2:
        leds_brightness = BADGE_BRIGHTNESS_0;
        break;
    }
    do_blink();
}

/// Start an animation in the eyes, optionally blinking before and after.
void leds_anim_start(eye_anim_t *animation, uint8_t blink_transition) {
    eye_anim_curr = animation;
    eye_anim_curr_frame = 0;
    eye_anim_curr_ticks = 0;
    eye_anim_curr_loops = eye_anim_curr->loop_count;
    eye_anim_blink_transition = blink_transition;

    eye_blinking = 0; // Clear any current blink

    if (blink_transition) {
        do_blink();
    } else {
        leds_eyes_curr[0] = animation->frames[0].eyes[0];
        leds_eyes_curr[1] = animation->frames[0].eyes[1];
        leds_load_gs();
    }
}

/// Display the boop animation.
void leds_boop() {
    if (eye_anim_curr != &anim_boop) {
        leds_eyes_ambient_temp[0] = HAPPY_RIGHT;
        leds_eyes_ambient_temp[1] = HAPPY_LEFT;
        leds_eyes_ambient_temp_ticks = BADGE_BOOP_FACE_LEN_CSECS;
        leds_anim_start(&anim_boop, 0);
    }
}

/// Raise an alert animation on the eyes, based on the `type` flag.
void leds_queerdar_alert(uint8_t type) {
    switch(type) {
    case LEDS_QUEERDAR_NEWBADGE:
        leds_anim_start(&anim_new_badge, 1);
        break;
    case LEDS_QUEERDAR_OLDBADGE:
        leds_anim_start(&anim_seen_badge, 1);
        break;
    case LEDS_QUEERDAR_PAIRBADGE:
        // Not implemented
        break;
    }
}

/// Display a POST error code based on the `code` flag.
void leds_error_code(uint8_t code) {
    switch(code) {
    case BADGE_POST_ERR_NONE:
        leds_eyes_curr[0] = CHAR_VERT_D_OR_O;
        leds_eyes_curr[1] = CHAR_VERT_K;
        break;
    case BADGE_POST_ERR_NOID:
        leds_eyes_curr[0] = CHAR_VERT_I;
        leds_eyes_curr[1] = CHAR_VERT_D_LOWER;
        break;
    case BADGE_POST_ERR_NORF:
        leds_eyes_curr[0] = CHAR_VERT_R;
        leds_eyes_curr[1] = CHAR_VERT_F;
        break;
    case BADGE_POST_ERR_FREQ:
        leds_eyes_curr[0] = CHAR_VERT_F;
        leds_eyes_curr[1] = CHAR_VERT_R_LOWER;
        break;
    }
    leds_load_gs();
}

/// Display a number [00..100], oriented 90 degrees from our usual angle.
void leds_show_number(uint8_t number, uint16_t make_temp_ambient) {
    if (number == 100) {
        leds_eyes_curr[0] = (eye_t) {0,1,0,1,0,1,1,1}; // i o
        leds_eyes_curr[1] = (eye_t) {1,1,1,1,0,0,0,0};//      o
    } else {
        leds_eyes_curr[0] = EYES_DIGITS[number/10];
        leds_eyes_curr[1] = EYES_DIGITS[number%10];
    }
    leds_load_gs();

    if (make_temp_ambient) {
        leds_eyes_ambient_temp_ticks = make_temp_ambient;
        leds_eyes_ambient_temp[0] = leds_eyes_curr[0];
        leds_eyes_ambient_temp[1] = leds_eyes_curr[1];
    }
}

/// Cycle through the LEDs, roughly left to right, to test them all.
void leds_post_step() {
    static uint8_t eye_index = 0;
    static uint8_t led_index = 0;

    leds_eyes_curr[!eye_index] = EYE_OFF;
    switch(led_index) {
    case 0:
        leds_eyes_curr[eye_index] = ONLY_L;
        break;
    case 1:
        leds_eyes_curr[eye_index] = ONLY_UL;
        break;
    case 2:
        leds_eyes_curr[eye_index] = ONLY_LL;
        break;
    case 3:
        leds_eyes_curr[eye_index] = ONLY_M;
        break;
    case 4:
        leds_eyes_curr[eye_index] = ONLY_UR;
        break;
    case 5:
        leds_eyes_curr[eye_index] = ONLY_LR;
        break;
    case 6:
        leds_eyes_curr[eye_index] = ONLY_R;
        break;
    case 7:
        leds_eyes_curr[eye_index] = ONLY_DOT;
        break;
    }

    leds_load_gs();

    led_index++;
    if (led_index > 7) {
        eye_index = !eye_index;
        led_index = 0;
    }
}

/// Perform a single timestep of the LED system. Call this at about 100 Hz.
void leds_timestep() {
    uint8_t update_eyes = 0;

    if (leds_scan_speed) {
        if (leds_dot_level[scan_dot_curr] > leds_brightness || leds_dot_level[!scan_dot_curr] > leds_brightness) {
            leds_dot_level[scan_dot_curr] = leds_brightness;
            leds_dot_level[!scan_dot_curr] = 0;
        }

        // Handle the scan transition
        uint16_t led_scan_step = leds_brightness / leds_scan_speed;

        // First decrement the active light
        if (led_scan_step >= leds_dot_level[scan_dot_curr]) {
            leds_dot_level[scan_dot_curr] = 0;
        } else {
            leds_dot_level[scan_dot_curr] -= led_scan_step;
        }
        // Then handle the target light, which doesn't start turning on
        //  until the active light is below a certain brightness
        if (leds_dot_level[scan_dot_curr] < leds_brightness/4) {
            // We need to increase the brightness
            if (leds_brightness - leds_scan_speed <= leds_dot_level[!scan_dot_curr]) {
                // Done increasing the brightness.
                // This is the last step, need to toggle the other way.
                scan_dot_curr = !scan_dot_curr;
                leds_dot_level[scan_dot_curr] = leds_brightness;
            } else {
                leds_dot_level[!scan_dot_curr] += led_scan_step;
            }
        }

        update_eyes = 1;
    }

    if (eye_blinking == 1) {
        // Just finished a blink
        eye_blinking = 0;

        // Restore either the ambient eyes or the first frame of the animation.
        if (!eye_anim_curr) {
            // Ambient
            leds_eyes_curr[0] = eye_ambient(0);
            leds_eyes_curr[1] = eye_ambient(1);
            update_eyes = 1;
        } else {
            // Animation
            leds_eyes_curr[0] = eye_anim_curr->frames[eye_anim_curr_frame].eyes[0];
            leds_eyes_curr[1] = eye_anim_curr->frames[eye_anim_curr_frame].eyes[1];
            update_eyes = 1;
        }
    } else if (eye_blinking) {
        // Tick down the blink timer.
        eye_blinking--;
    } else if (eye_anim_curr) {
        // We're not blinking, but we are animating.
        // Are we done with the current frame?
        if (eye_anim_curr_ticks >= eye_anim_curr->frames[eye_anim_curr_frame].dur) {
            // Yes, done with the current frame.
            eye_anim_curr_ticks = 0;
            // Is this the last frame?
            if (eye_anim_curr_frame+1 == eye_anim_curr->length && eye_anim_curr_loops) {
                // Yes, but we have loops left to do.
                eye_anim_curr_loops--;
                eye_anim_curr_frame = 0;
                eye_anim_curr_ticks = 0;
                leds_eyes_curr[0] = eye_anim_curr->frames[eye_anim_curr_frame].eyes[0];
                leds_eyes_curr[1] = eye_anim_curr->frames[eye_anim_curr_frame].eyes[1];
                // Update the eyes.
                update_eyes = 1;
            } else if (eye_anim_curr_frame+1 == eye_anim_curr->length) {
                // Yes, and there are no (more) loops to do.
                eye_anim_curr = 0x0000;
                if (eye_anim_blink_transition) {
                    do_blink();
                    // Don't update the eyes.
                    update_eyes = 0;
                } else {
                    leds_eyes_curr[0] = eye_ambient(0);
                    leds_eyes_curr[1] = eye_ambient(1);
                    // Update the eyes.
                    update_eyes = 1;
                }
            } else {
                // Not done with the animation, so load next frame.
                eye_anim_curr_frame++;
                leds_eyes_curr[0] = eye_anim_curr->frames[eye_anim_curr_frame].eyes[0];
                leds_eyes_curr[1] = eye_anim_curr->frames[eye_anim_curr_frame].eyes[1];
                // Update the eyes.
                update_eyes = 1;
            }
        } else {
            // Not done with the current frame. Nothing to do.
        }
        // Regardless, we increment the ticks.
        eye_anim_curr_ticks++;
    } else {
        // We're just ambient.
        // Are we temp-ambient?
        if (leds_eyes_ambient_temp_ticks == 1) {
            leds_eyes_ambient_temp_ticks = 0;
        } else if (leds_eyes_ambient_temp_ticks) {
            leds_eyes_ambient_temp_ticks--;
        }
    }

    if (update_eyes) {
        leds_load_gs();
    }
}

/// Called when it's time for the eyes to blink or animate.
/**
 * There's a 1 in `BADGE_ANIM_CHANCE_ONE_IN` chance that it will
 * choose an eye animation and start that. If it does an animation,
 * there's a further 1 in `BADGE_FACE_CHANCE_ONE_IN` chance that
 * it will select new ambient eyes for after the animation finishes.
 *
 * Otherwise, it will just blink.
 */
void leds_blink_or_bling() {
    // Don't blink or start new animation in the middle of an existing one.
    if (eye_anim_curr)
        return;

    if (rand() % BADGE_ANIM_CHANCE_ONE_IN == 0) {
        // Make an animated face!
        leds_anim_start(animations[rand() % ANIMATION_COUNT], 1);
        if (rand() % BADGE_FACE_CHANCE_ONE_IN == 0) {
            // Decide to change our ambient face
            fram_unlock();
            leds_eyes_ambient = rand() % EYES_COUNT;
            fram_lock();
        }
    } else {
        do_blink();
    }
}

/// Initialize the LED driver system, including the low-level TLC5948A driver.
void leds_init() {
    tlc_init();

    leds_eyes_curr[0] = (eye_t) {0, 0, 0, 1, 0, 1, 1, 1};
    leds_eyes_curr[1] = (eye_t) {1, 1, 1, 1, 0, 0, 0, 0};

    leds_load_gs();
}
