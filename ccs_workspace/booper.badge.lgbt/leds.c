/*
 * leds.c
 *
 *  Created on: Jul 31, 2023
 *      Author: george
 */

#include <stdlib.h>

#include "badge.h"
#include "leds.h"
#include "tlc5948a.h"
#include "animations.h"
#include "eyes.h"

// General configuration of the 7-segs
/// The standard brightness of the LEDs.
uint16_t leds_brightness = 0x0fff;

// Eye animations
/// The current display configuration of the eyes.
eye_t leds_eyes_curr[2] = {0, }; // Start all off
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

/// Number of LED timesteps remaining in the current blink animation
uint8_t eye_blinking = 0;

// Dot-scanner animations
/// Current scan speed, where 0 is off but otherwise higher is slower. 80 = about 1 second.
uint16_t leds_scan_speed = 24;
/// The brightness of the scan dots, although this can be overridden by leds_eyes_curr.
uint16_t leds_dot_level[] = {0x0000, 0x0000};
/// Which eye's scan dot is currently on (and fading out).
uint8_t scan_dot_curr = 0;

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
        } else {
            // Otherwise, follow their dot level variable.
            tlc_gs_data[eye*8 + 4] = leds_dot_level[eye];
        }
    }

    tlc_set_gs();
}

void do_blink() {
    eye_blinking = 30;
    leds_eyes_curr[0] = EYE_OFF;
    leds_eyes_curr[1] = EYE_OFF;

    for (uint8_t eye=0; eye<2; eye++) {
        if (EYES_DISP[leds_eyes_ambient][eye].bl || EYES_DISP[leds_eyes_ambient][eye].l || EYES_DISP[leds_eyes_ambient][eye].tl) {
            leds_eyes_curr[eye].bl = 1;
        }
        if (EYES_DISP[leds_eyes_ambient][eye].br || EYES_DISP[leds_eyes_ambient][eye].r || EYES_DISP[leds_eyes_ambient][eye].tr) {
            leds_eyes_curr[eye].br = 1;
        }
    }

    leds_load_gs();
}

void leds_timestep() {
    uint8_t update_eyes = 0;

    if (leds_scan_speed) {
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


    if (eye_blinking) {
        // Tick down the blink timer.
        eye_blinking--;

        // If it's done, and there's no animation going, restore ambient.
        if (!eye_blinking && !eye_anim_curr) {
            leds_eyes_curr[0] = EYES_DISP[leds_eyes_ambient][0];
            leds_eyes_curr[1] = EYES_DISP[leds_eyes_ambient][1];
            update_eyes = 1;
        } else if (eye_anim_curr) {
            leds_eyes_curr[0] = eye_anim_curr->frames[eye_anim_curr_frame].eyes[0];
            leds_eyes_curr[1] = eye_anim_curr->frames[eye_anim_curr_frame].eyes[1];
            update_eyes = 1;
        }
        // If there is an animation going, it will return on the next LED timestep.
    } else if (eye_anim_curr) {
        // Yes.
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
                    leds_eyes_curr[0] = EYES_DISP[leds_eyes_ambient][0];
                    leds_eyes_curr[1] = EYES_DISP[leds_eyes_ambient][1];
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
    }

    if (update_eyes) {
        leds_load_gs();
    }
}

void leds_anim_start(eye_anim_t *animation, uint8_t blink_transition) {
    eye_anim_curr = animation;
    eye_anim_curr_frame = 0;
    eye_anim_curr_ticks = 0;
    eye_anim_curr_loops = eye_anim_curr->loop_count;
    eye_anim_blink_transition = blink_transition;

    leds_eyes_curr[0] = animation->frames[0].eyes[0];
    leds_eyes_curr[1] = animation->frames[0].eyes[1];
    if (blink_transition) {
        do_blink();
    } else {
        leds_load_gs();
    }
}

void leds_blink_or_bling() {
    // Don't blink or start new animation in the middle of an existing one.
    if (eye_anim_curr)
        return;

    if (rand() % 8 == 0) { // TODO: values
        // Make an animated face!
        leds_anim_start(animations[rand() % ANIMATION_COUNT], 1);
        if (rand() % 8 == 0) { // TODO: values
            // Decide to change our ambient face
            leds_eyes_ambient = rand() % EYES_COUNT;
        }
    } else {
        do_blink();
    }
}

void leds_boop() {
}

void leds_init() {
    tlc_init();

    leds_eyes_curr[0] = (eye_t) {0, 0, 0, 1, 0, 1, 1, 1};
    leds_eyes_curr[1] = (eye_t) {1, 1, 1, 1, 0, 0, 0, 0};

    leds_load_gs();
}
