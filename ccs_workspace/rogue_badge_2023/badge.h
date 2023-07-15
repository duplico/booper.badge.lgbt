/// mini.badge.lgbt 2023 main header file.
/**
 ** This header contains the core configuration and functionality declarations
 ** and definitions for the Queercon 2022 badge.
 **
 ** \file badge.h
 ** \author George Louthan
 ** \date   2022
 ** \copyright (c) 2022 George Louthan @duplico. MIT License.
 */

#ifndef BADGE_H_
#define BADGE_H_

/// MCLK rate in MHZ.
#define MCLK_FREQ_MHZ 8
/// SMCLK rate in Hz.
#define SMCLK_RATE_HZ 8000000

extern uint8_t badge_brightness_level;
extern volatile uint8_t f_time_loop;
extern volatile uint8_t f_long_press;
extern volatile uint8_t f_second;
extern uint8_t button_state;

#endif /* BADGE_H_ */
