/// booper.badge.lgbt 2023 utility function source file.
/**
 **
 ** \file util.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2018-2023 George Louthan @duplico. MIT License.
 */

#include <msp430.h>
#include <driverlib.h>

#include "util.h"
#include "badge.h"

void delay_millis(unsigned long mils) {
    while (mils) {
        __delay_cycles(MCLK_FREQ_MHZ*1000);
        mils--;
    }
}

/// Compute a 16-bit CRC on `len` bytes of byte buffer `buf`.
uint16_t crc16_compute(uint8_t *buf, uint16_t len) {
    CRC_setSeed(CRC_BASE, CRC16_SEED);
    for (uint16_t i=0; i<len; i++) {
        CRC_set8BitData(CRC_BASE, buf[i]);
    }
    return CRC_getResult(CRC_BASE);
}

/// Append a 16-bit CRC onto the end of a `len`-byte buffer `buf`.
/**
 ** Note that `len` does NOT include the CRC. This matches the Endianness of
 ** the MSP430 device, so if `buf` points to a packed struct whose last
 ** member is a 16-bit integer, that integer will match the result of
 ** calling crc16_compute(buf, sizeof(struct)-2).
 */
void crc16_append_buffer(uint8_t *buf, uint16_t len) {
    uint16_t crc = crc16_compute(buf, len);
    buf[len] = crc & 0xFF;
    buf[len + 1] = (crc >> 8) & 0xFF;
}

/// Check whether a 16-bit CRC is at the end of a `len`-byte buffer `buf`.
/**
 ** Note that there must be room in the buffer AFTER len for the CRC.
 */
uint8_t crc16_check_buffer(uint8_t *buf, uint16_t len) {
    uint16_t crc = crc16_compute(buf, len);
    return (buf[len] == (crc & 0xFF)) && (buf[len+1] == ((crc >> 8) & 0xFF));
}

/// Given a standard buffer of bitfields, check whether ``id``'s bit is set.
uint8_t check_id_buf(uint16_t id, uint8_t *buf) {
    uint8_t byte;
    uint8_t bit;
    byte = id / 8;
    bit = id % 8;
    return (buf[byte] & (BIT0 << bit)) ? 1 : 0;
}

/// In a standard buffer of bitfields, set ``id``'s bit to 1.
void set_id_buf(uint16_t id, uint8_t *buf) {
    uint8_t byte;
    uint8_t bit;
    byte = id / 8;
    bit = id % 8;
    buf[byte] |= (BIT0 << bit);
}

/// In a standard buffer of bitfields, set ``id``'s bit to 0.
void unset_id_buf(uint16_t id, uint8_t *buf) {
    uint8_t byte;
    uint8_t bit;
    byte = id / 8;
    bit = id % 8;
    buf[byte] &= ~(BIT0 << bit);
}

/// Counts the bits set in a byte and return the total.
/**
 ** This is the Brian Kernighan, Peter Wegner, and Derrick Lehmer way of
 ** counting bits in a bitstring. See _The C Programming Language_, 2nd Ed.,
 ** Exercise 2-9; or _CACM 3_ (1960), 322.
 */
uint8_t byte_rank(uint8_t v) {
    uint8_t c;
    for (c = 0; v; c++) {
        v &= v - 1; // clear the least significant bit set
    }
    return c;
}

/// Counts the bits set in all the bytes of a buffer and returns it.
/**
 ** This is the Brian Kernighan, Peter Wegner, and Derrick Lehmer way of
 ** counting bits in a bitstring. See _The C Programming Language_, 2nd Ed.,
 ** Exercise 2-9; or _CACM 3_ (1960), 322.
 */
uint16_t buffer_rank(uint8_t *buf, uint8_t len) {
    uint16_t count = 0;
    uint8_t c, v;
    for (uint8_t i=0; i<len; i++) {
        v = buf[i];
        for (c = 0; v; c++) {
            v &= v - 1; // clear the least significant bit set
        }
        count += c;
    }
    return count;
}
