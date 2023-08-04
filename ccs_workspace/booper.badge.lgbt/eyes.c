/// Definitions of arrays of eye layouts for 2023 booper.badge.lgbt.
/**
 ** This module includes anything that needs to be tied together to
 ** make a reusable face. See the header file for definitions of
 ** the single-eye shapes, which are done in the preprocessor mostly.
 **
 ** \file eyes.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2023 George Louthan @duplico. MIT License.
 */

#include "leds.h"
#include "eyes.h"

eye_t EYES_NORMAL_VAL[2] = {CIRCLE_RIGHT, CIRCLE_LEFT};
eye_t EYES_HAPPY_VAL[2] = {HAPPY_LEFT, HAPPY_RIGHT};
eye_t EYES_SAD_VAL[2] = {SAD_RIGHT, SAD_LEFT};
eye_t EYES_BIG_VAL[2] = {CIRCLE_BIG, CIRCLE_BIG};
eye_t EYES_LEFT_VAL[2] = {SIDE_LEFT, SIDE_LEFT};
eye_t EYES_RIGHT_VAL[2] = {SIDE_RIGHT, SIDE_RIGHT};
eye_t EYES_BIGHAPPY_VAL[2] = {HAPPY_BIG, HAPPY_BIG};
eye_t EYES_DAFUQ_VAL[2] = {CIRCLE_BIG, CIRCLE_LEFT};

eye_t *EYES_DISP[] = {
    EYES_NORMAL_VAL,
    EYES_HAPPY_VAL,
    EYES_SAD_VAL,
    EYES_BIG_VAL,
    EYES_LEFT_VAL,
    EYES_RIGHT_VAL,
    EYES_BIGHAPPY_VAL,
    EYES_DAFUQ_VAL,
};

eye_t EYES_DIGITS[] = {
    NUMBER_ZERO,
    NUMBER_ONE,
    NUMBER_TWO,
    NUMBER_THREE,
    NUMBER_FOUR,
    NUMBER_FIVE,
    NUMBER_SIX,
    NUMBER_SEVEN,
    NUMBER_EIGHT,
    NUMBER_NINE,
};
