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
    EYES_COUNT
} eyes_disp_t;

#define CIRCLE_LEFT (eye_t) {1, 1, 1, 1, 0, 0, 0, 0}
#define CIRCLE_RIGHT (eye_t) {0, 0, 0, 1, 0, 1, 1, 1}

#define HAPPY_LEFT (eye_t) {1, 1, 0, 1, 0, 0, 0, 0}
#define HAPPY_RIGHT (eye_t) {0, 0, 0, 1, 0, 1, 1, 0}

#define BLINK_RIGHT (eye_t) {0,0,0,0,0,0,0,1}
#define BLINK_LEFT (eye_t)  {0,0,1,0,0,0,0,0}
#define BLINK_WIDE (eye_t)  {0,0,1,0,0,0,0,1}
#define BLINK_MIDDLE (eye_t) {0,0,0,1,0,0,0,0}

extern eye_t *EYES_DISP[];

#endif /* EYES_H_ */
