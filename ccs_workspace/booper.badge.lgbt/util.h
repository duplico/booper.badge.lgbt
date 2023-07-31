/// booper.badge.lgbt 2023 utility function header file.
/**
 **
 ** \file util.h
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2018-2023 George Louthan @duplico. MIT License.
 */

#ifndef UTIL_H_
#define UTIL_H_

#define CRC16_SEED 0x9C8B

void delay_millis(unsigned long mils);

uint16_t crc16_compute(uint8_t *buf, uint16_t len);
void crc16_append_buffer(uint8_t *buf, uint16_t len);
uint8_t crc16_check_buffer(uint8_t *buf, uint16_t len);
uint8_t check_id_buf(uint16_t id, uint8_t *buf);
void set_id_buf(uint16_t id, uint8_t *buf);
void unset_id_buf(uint16_t id, uint8_t *buf);
uint16_t buffer_rank(uint8_t *buf, uint8_t len);
uint8_t byte_rank(uint8_t v);

#endif /* UTIL_H_ */
