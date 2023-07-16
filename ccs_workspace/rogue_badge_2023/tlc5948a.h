/// Low-level driver header for TI TLC5948A PWM LED controller.
/**
 ** Header for the low-level LED driver. It also contains some hardware-level
 ** configuration in preprocessor defines.
 **
 ** \file tlc5948a.h
 ** \author George Louthan
 ** \date   2022
 ** \copyright (c) 2015-2023 George Louthan @duplico. MIT License.
 */

#ifndef TLC5948A_H_
#define TLC5948A_H_

#include <stdint.h>

// Defines for pins:

/// The DriverLib GPIO_PORT for the LATCH line.
#define TLC_LATPORT GPIO_PORT_P1
/// The pin for the LATCH line, DriverLib style.
#define TLC_LATPIN  GPIO_PIN7

/// The built-in register PxOUT for the LATCH line.
#define LAT_POUT    P1OUT
/// The built-in pin bit in register LAT_POUT for the LATCH line.
#define LAT_PBIT    BIT7

/// Idle state of the low-level SPI state machine.
#define TLC_SEND_IDLE     0
/// Grayscale sending state of the low-level SPI state machine.
#define TLC_SEND_TYPE_GS  1
/// Function data sending state of the low-level SPI state machine.
#define TLC_SEND_TYPE_FUN 2
/// Loopback testing state of the low-level SPI state machine.
#define TLC_SEND_TYPE_LB  3

extern uint8_t tlc_send_type;
extern uint16_t tlc_gs_data[16];

void tlc_init();
uint8_t tlc_test_loopback(uint8_t);
void tlc_set_gs();
void tlc_set_fun();
void tlc_stage_dc_mult(uint8_t mult);
void tlc_stage_bc(uint8_t bc);
void tlc_stage_blank(uint8_t);

#endif /* TLC5948A_H_ */
