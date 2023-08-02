/*
 * animations.c
 *
 *  Created on: Aug 1, 2023
 *      Author: george
 */

#include "leds.h"
#include "eyes.h"

eye_anim_frame_t anim_shifty_frames[] = {
    (eye_anim_frame_t) {
        {CIRCLE_LEFT, CIRCLE_LEFT},
        20,
    },
    (eye_anim_frame_t) {
        {CIRCLE_RIGHT, CIRCLE_RIGHT},
        20,
    }
};

eye_anim_t anim_shifty = (eye_anim_t) {
    .frames = anim_shifty_frames,
    .length = 2,
    .loop_count = 3,
};

eye_anim_t *animations[] = {
    &anim_shifty,
};
