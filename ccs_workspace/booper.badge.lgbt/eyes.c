/*
 * eyes.c
 *
 *  Created on: Aug 1, 2023
 *      Author: george
 */

#include "leds.h"
#include "eyes.h"

eye_t EYES_NORMAL_VAL[2] = {CIRCLE_RIGHT, CIRCLE_LEFT};
eye_t EYES_HAPPY_VAL[2] = {HAPPY_RIGHT, HAPPY_LEFT};

eye_t *EYES_DISP[] = {
    EYES_NORMAL_VAL,
    EYES_HAPPY_VAL,
};
