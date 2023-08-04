/*
 * eyes.h
 *
 *  Created on: Aug 1, 2023
 *      Author: george
 */

#ifndef EYES_H_
#define EYES_H_

typedef enum {
    EYES_NORMAL = 0,
    EYES_HAPPY,
    EYES_SAD,
    EYES_BIG,
    EYES_LEFT,
    EYES_RIGHT,
    EYES_BIGHAPPY,
    EYES_DAFUQ,
    EYES_COUNT
} eyes_disp_t;

/*       4 (dot)
 *  0 5
 * 1 3 6
 *  2 7
 */

#define EYE_OFF (eye_t) {0,0,0,0,0,0,0,0}

#define CIRCLE_LEFT (eye_t) {1, 1, 1, 1, 0, 0, 0, 0}
#define CIRCLE_RIGHT (eye_t) {0, 0, 0, 1, 0, 1, 1, 1}
#define CIRCLE_BIG (eye_t) {1,1,1,0,0,1,1,1}

#define HAPPY_LEFT (eye_t) {1, 1, 0, 1, 0, 0, 0, 0}
#define HAPPY_RIGHT (eye_t) {0, 0, 0, 1, 0, 1, 1, 0}
#define HAPPY_BIG (eye_t) {1,1,0,0,0,1,1,0}

#define SAD_LEFT (eye_t) {0,1,1,1,0,0,0,0}
#define SAD_RIGHT (eye_t) {0,0,0,1,0,0,1,1}

#define SIDE_LEFT (eye_t) {1,1,1,0,0,0,0}
#define SIDE_RIGHT (eye_t) {0,0,0,0,0,1,1,1}

#define BLINK_RIGHT (eye_t) {0,0,0,0,0,0,0,1}
#define BLINK_LEFT (eye_t)  {0,0,1,0,0,0,0,0}
#define BLINK_WIDE (eye_t)  {0,0,1,0,0,0,0,1}
#define BLINK_MIDDLE (eye_t) {0,0,0,1,0,0,0,0}

#define DIAG_UL (eye_t) {1,1,0,0,0,0,0,0}
#define DIAG_LL (eye_t) {0,1,1,0,0,0,0,0}
#define DIAG_UR (eye_t) {0,0,0,0,0,1,1,0}
#define DIAG_LR (eye_t) {0,0,0,0,0,0,1,1}

#define ONLY_UL (eye_t) {1,0,0,0,0,0,0,0}
#define ONLY_L  (eye_t) {0,1,0,0,0,0,0,0}
#define ONLY_LL (eye_t) {0,0,1,0,0,0,0,0}
#define ONLY_M  (eye_t) {0,0,0,1,0,0,0,0}
#define ONLY_UR (eye_t) {0,0,0,0,0,1,0,0}
#define ONLY_R  (eye_t) {0,0,0,0,0,0,1,0}
#define ONLY_LR (eye_t) {0,0,0,0,0,0,0,1}
#define ONLY_DOT (eye_t) {0,0,0,0,1,0,0,0}

#define CHAR_VERT_D_OR_O (eye_t) {1,1,1,0,0,1,1,1}
#define CHAR_VERT_K (eye_t) {1,0,1,1,0,1,0,1}
#define CHAR_VERT_I (eye_t) {0,0,1,0,0,0,0,1}
#define CHAR_VERT_R (eye_t) {1,1,1,1,0,1,0,1}
#define CHAR_VERT_R_LOWER (eye_t) {0,0,0,1,0,0,0,1}
#define CHAR_VERT_F (eye_t) {0,1,1,1,0,0,0,1}

extern eye_t *EYES_DISP[];

#endif /* EYES_H_ */
