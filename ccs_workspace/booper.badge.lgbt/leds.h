/// Application-level LED driver header for booper.badge.lgbt.
/**
 ** This module is used for driving the application-specific behavior
 ** of the LEDs for the badge. For the actual hardware and peripheral
 ** interface with the TLC5948A LED driver module, see the low-level
 ** driver, tlc5948a.c and tlc5948a.h.
 **
 ** \file leds.h
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2023 George Louthan @duplico. MIT License.
 */

#ifndef LEDS_H_
#define LEDS_H_

#include <stdint.h>

typedef struct {
    uint8_t tl : 1;
    uint8_t l : 1;
    uint8_t bl : 1;
    uint8_t m : 1;
    uint8_t dot : 1;
    uint8_t tr : 1;
    uint8_t r : 1;
    uint8_t br : 1;
} eye_t;

typedef struct {
    eye_t eyes[2];
    uint8_t dur;
} eye_anim_frame_t;

typedef struct {
    eye_anim_frame_t *frames;
    uint8_t loop_count;
    uint8_t length;
} eye_anim_t;

extern uint16_t leds_brightness;
extern uint16_t leds_scan_speed;

#define LEDS_QUEERDAR_NEWBADGE 0
#define LEDS_QUEERDAR_OLDBADGE 1
#define LEDS_QUEERDAR_PAIRBADGE 2

void leds_post_step();
void leds_next_brightness();
void leds_error_code(uint8_t code);
void leds_show_number(uint8_t number);
void leds_timestep();
void leds_blink_or_bling();
void leds_boop();
void leds_queerdar_alert(uint8_t type);
void leds_init();

#endif /* LEDS_H_ */
