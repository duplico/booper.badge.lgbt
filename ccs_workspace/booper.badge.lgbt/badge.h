/// mini.badge.lgbt 2023 main header file.
/**
 ** This header contains the core configuration and functionality declarations
 ** and definitions for the booper.badge.lgbt badge.
 **
 ** \file badge.h
 ** \author George Louthan
 ** \date   2022
 ** \copyright (c) 2022 George Louthan @duplico. MIT License.
 */

#ifndef BADGE_H_
#define BADGE_H_

#include "stdint.h"

/// MCLK rate in MHZ.
#define MCLK_FREQ_MHZ 8
/// SMCLK rate in Hz.
#define SMCLK_RATE_HZ 8000000

/// Number of csecs for a button long-press
#define BUTTON_LONG_PRESS_CSECS 150

/// Number of possible badges in the system
#define BADGES_IN_SYSTEM 120

/// Number of bytes in the bitfield of all badge IDs
#define BADGES_SEEN_BUFFER_LEN_BYTES BADGES_IN_SYSTEM / 8 + 1
/// Valid badge ID but indicating it hasn't been assigned by a controller.
#define BADGE_ID_UNASSIGNED 250

/// The number of seconds allowed between radio boops
#define BADGE_RADIO_BOOP_COOLDOWN 2

/// Duration of a blink in csec ticks.
#define BLINK_TICKS 30

#define BADGE_BRIGHTNESS_0 0x00ff
#define BADGE_BRIGHTNESS_1 0x0900
#define BADGE_BRIGHTNESS_2 0xf000

#define BADGE_POST_ERR_NONE 0
#define BADGE_POST_ERR_NOID 1
#define BADGE_POST_ERR_NORF 2
#define BADGE_POST_ERR_FREQ 3

#define BADGE_RADIO_CALIBRATION_SECS_PER_FREQ 8
#define BADGE_RADIO_CALIBRATION_SECS_PER_FREQ_INITIAL 4
#define BADGE_SECS_PER_BLINK_AVG 5
#define BADGE_BOOP_RADIO_HOPS 10
#define BADGE_BOOP_FACE_LEN_CSECS 800
#define BADGE_ANIM_CHANCE_ONE_IN 8
#define BADGE_FACE_CHANCE_ONE_IN 8

/// Badge config struct definition
typedef struct {
    /// The badge's ID, between 0 and BADGE_ID_UNASSIGNED, inclusive.
    uint16_t badge_id;
    /// Bitfield tracking badge IDs seen.
    uint8_t badges_seen[BADGES_SEEN_BUFFER_LEN_BYTES];
    /// Counter of badges seen generally
    uint8_t badges_seen_count;
    /// Has my setup been completed?
    uint8_t bootstrapped;
} badge_conf_t;

extern volatile badge_conf_t badge_conf;
extern uint8_t badge_block_radio_game;

extern uint8_t badge_brightness_level;
extern volatile uint8_t f_time_loop;
extern volatile uint8_t f_button_press_long;
extern volatile uint8_t f_second;
extern uint8_t s_boop_radio;
extern volatile uint8_t button_state;

extern uint8_t badge_boop_radio_cooldown;

extern uint8_t long_presses;

volatile void fram_unlock();
volatile void fram_lock();

void badge_update_queerdar_count(uint8_t badges_nearby);
void badge_set_seen(uint8_t id);
void badge_button_press_long();
void badge_button_press_short();

void badge_init();

#endif /* BADGE_H_ */
