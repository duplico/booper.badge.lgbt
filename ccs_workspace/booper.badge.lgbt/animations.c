/*
 * animations.c
 *
 *  Created on: Aug 1, 2023
 *      Author: george
 */

#include "leds.h"
#include "eyes.h"

eye_anim_frame_t anim_shifty[] = {
    (eye_anim_frame_t) {
        {CIRCLE_LEFT, CIRCLE_LEFT},
        20,
        0
    },
    (eye_anim_frame_t) {
        {CIRCLE_RIGHT, CIRCLE_RIGHT},
        20,
        0
    },
    (eye_anim_frame_t) {
        {CIRCLE_LEFT, CIRCLE_LEFT},
        20,
        0
    },
    (eye_anim_frame_t) {
        {CIRCLE_RIGHT, CIRCLE_RIGHT},
        20,
        0
    },
    (eye_anim_frame_t) {
        {CIRCLE_LEFT, CIRCLE_LEFT},
        20,
        0
    },
    (eye_anim_frame_t) {
        {CIRCLE_RIGHT, CIRCLE_RIGHT},
        20,
        0
    },
    (eye_anim_frame_t) {
        {CIRCLE_LEFT, CIRCLE_LEFT},
        20,
        0
    },
    (eye_anim_frame_t) {
        {CIRCLE_RIGHT, CIRCLE_RIGHT},
        20,
        1
    },
};

eye_anim_frame_t *animations[] = {
    anim_shifty,
};
