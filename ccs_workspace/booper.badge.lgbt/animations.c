/// Eye animation definitions for 2023 booper.badge.lgbt.
/**
 **
 **
 ** \file animations.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2023 George Louthan @duplico. MIT License.
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

eye_anim_frame_t anim_dafuq_frames[] = {
    (eye_anim_frame_t) {
        {CIRCLE_RIGHT, CIRCLE_BIG},
        50,
    },
    (eye_anim_frame_t) {
        {CIRCLE_BIG, CIRCLE_LEFT},
        50,
    },
};

eye_anim_t anim_dafuq = (eye_anim_t) {
    .frames = anim_dafuq_frames,
    .length = 2,
    .loop_count = 0,
};

eye_anim_frame_t anim_happytoggle_frames[] = {
    (eye_anim_frame_t) {
        {HAPPY_LEFT, HAPPY_RIGHT},
        75,
    },
    (eye_anim_frame_t) {
        {HAPPY_RIGHT, HAPPY_LEFT},
        75,
    },
};

eye_anim_t anim_happytoggle = (eye_anim_t) {
    .frames = anim_happytoggle_frames,
    .length = 2,
    .loop_count = 6,
};

eye_anim_frame_t anim_happywink_frames[] = {
    (eye_anim_frame_t) {
        {HAPPY_BIG, BLINK_WIDE},
        150,
    },
};

eye_anim_t anim_happywink = (eye_anim_t) {
    .frames = anim_happywink_frames,
    .length = 1,
    .loop_count = 0,
};

eye_anim_frame_t anim_wink_frames[] = {
    (eye_anim_frame_t) {
        {CIRCLE_RIGHT, BLINK_LEFT},
        80,
    },
};

eye_anim_t anim_wink = (eye_anim_t) {
    .frames = anim_wink_frames,
    .length = 1,
    .loop_count = 0,
};

eye_anim_frame_t anim_dirshifty_frames[] = {
    (eye_anim_frame_t) {
        {SIDE_LEFT, SIDE_LEFT},
        45,
    },
    (eye_anim_frame_t) {
        {SIDE_RIGHT, SIDE_RIGHT},
        45,
    }
};

eye_anim_t anim_dirshifty = (eye_anim_t) {
    .frames = anim_dirshifty_frames,
    .length = 2,
    .loop_count = 3,
};


eye_anim_frame_t anim_lookaround_frames[] = {
    (eye_anim_frame_t) {
        {DIAG_UL, DIAG_UL},
        32,
    },
    (eye_anim_frame_t) {
        {DIAG_LR, DIAG_LR},
        32,
    },
};

eye_anim_t anim_lookaround = (eye_anim_t) {
    .frames = anim_lookaround_frames,
    .length = 2,
    .loop_count = 4,
};

eye_anim_t *animations[] = {
    &anim_shifty,
    &anim_dafuq,
    &anim_happytoggle,
    &anim_happywink,
    &anim_wink,
    &anim_dirshifty,
    &anim_lookaround,
};

eye_anim_frame_t anim_boop_frames[] = {
   (eye_anim_frame_t) {
       {ONLY_M, ONLY_L},
       5,
   },
   (eye_anim_frame_t) {
       {ONLY_UR, ONLY_UL},
       5,
   },
   (eye_anim_frame_t) {
       {ONLY_R, ONLY_M},
       5,
   },
   (eye_anim_frame_t) {
       {ONLY_LR, ONLY_LL},
       5,
   },
};

eye_anim_t anim_boop = (eye_anim_t) {
    .frames = anim_boop_frames,
    .length = 4,
    .loop_count = 10,
};

#define BIGSPIN_FRAMERATE 3
eye_anim_frame_t anim_bigspin_frames[] = {
    (eye_anim_frame_t) {
        {ONLY_L, EYE_OFF},
        BIGSPIN_FRAMERATE,
    },
    (eye_anim_frame_t) {
        {ONLY_LL, EYE_OFF},
        BIGSPIN_FRAMERATE,
    },
    (eye_anim_frame_t) {
        {ONLY_LR, EYE_OFF},
        BIGSPIN_FRAMERATE,
    },
    (eye_anim_frame_t) {
        {EYE_OFF, ONLY_LL},
        BIGSPIN_FRAMERATE,
    },
    (eye_anim_frame_t) {
        {EYE_OFF, ONLY_LR},
        BIGSPIN_FRAMERATE,
    },
    (eye_anim_frame_t) {
        {EYE_OFF, ONLY_R},
        BIGSPIN_FRAMERATE,
    },
    (eye_anim_frame_t) {
        {EYE_OFF, ONLY_UR},
        BIGSPIN_FRAMERATE,
    },
    (eye_anim_frame_t) {
        {EYE_OFF, ONLY_UL},
        BIGSPIN_FRAMERATE,
    },
    (eye_anim_frame_t) {
        {ONLY_UR, EYE_OFF},
        BIGSPIN_FRAMERATE,
    },
    (eye_anim_frame_t) {
        {ONLY_UL, EYE_OFF},
        BIGSPIN_FRAMERATE,
    },
};

eye_anim_t anim_new_badge = (eye_anim_t) {
    .frames = anim_bigspin_frames,
    .length = 10,
    .loop_count = 8,
};

eye_anim_t anim_seen_badge = (eye_anim_t) {
    .frames = anim_bigspin_frames,
    .length = 10,
    .loop_count = 1,
};
