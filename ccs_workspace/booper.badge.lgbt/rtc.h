/// Header for real-time clock module.
/**
 ** \file rtc.h
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2022-23 George Louthan @duplico. MIT License.
 */

#ifndef RTC_H_
#define RTC_H_

extern volatile uint32_t rtc_seconds;
extern volatile uint8_t rtc_centiseconds;
extern uint8_t rtc_button_csecs;
extern uint8_t rtc_clock_authority;

inline void rtc_set_time(uint32_t clock, uint8_t authority);
void rtc_init();

#endif /* RTC_H_ */
