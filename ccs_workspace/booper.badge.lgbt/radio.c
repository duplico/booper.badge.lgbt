/// App-level driver for HopeRF RFM75 module.
/**
 **
 **
 ** \file radio.c
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2018-2023 George Louthan @duplico. MIT License.
 */
#include <stdint.h>

#include <msp430fr2633.h>

#include "badge.h"
#include "util.h"
#include "radio.h"
#include "rfm75.h"
#include "rtc.h"
#include "leds.h"

/// An array of all badges and we can currently see.
badge_info_t ids_in_range[BADGES_IN_SYSTEM] = {0};
/// The current radio packet we're sending (or just sent).
radio_proto_t curr_packet_tx;

uint16_t rx_cnt[FREQ_NUM] = {0,};

#pragma PERSISTENT(radio_frequency)
uint8_t radio_frequency = FREQ_MIN + FREQ_NUM / 2;
#pragma PERSISTENT(radio_frequency_done)
uint8_t radio_frequency_done = 1; // TODO

/// Current count of badges in range, not including ourself.
uint8_t radio_badges_in_range = 0;

uint8_t validate(radio_proto_t *msg, uint8_t len) {
    if (len != sizeof(radio_proto_t)) {
        // PROBLEM
        return 0;
    }

    // Check for bad ID:
    if (msg->badge_id >= BADGES_IN_SYSTEM && msg->badge_id != BADGE_ID_UNASSIGNED) // TODO: Base?
        return 0;

    // Finally, verify the CRC:
    return (crc16_check_buffer((uint8_t *) msg, len-2));
}

void radio_handle_beacon(uint16_t id) {
    // We've received a radio beacon.
    if (id == BADGE_ID_UNASSIGNED) {
        // TODO: Handle unassigned.
        return;
    }
    // Here, the ID is valid.
    if (id == badge_conf.badge_id) {
        // If it's a duplicate of the local badge, ignore it.
        return;
    }
    if (!ids_in_range[id].intervals_left) {
        // This badge is not currently in range.
        // Tell the badge system to mark it as newly in range.
        radio_badges_in_range++;
        badge_update_queerdar_count(radio_badges_in_range);
        badge_set_seen(id);
    }
    // Mark it as recently seen.
    ids_in_range[id].intervals_left = RADIO_WINDOW_BEACON_COUNT;
}

/// Called when the transmission of `curr_packet` has either finished or failed.
void radio_tx_done(uint8_t ack) {
    switch(curr_packet_tx.msg_type) {
        case RADIO_MSG_TYPE_BEACON:
            // We just sent a beacon.
            // There's no state that needs to be cleared at this point.
            break;
    }
}

void radio_rx_done(uint8_t* data, uint8_t len, uint8_t pipe) {
    radio_proto_t *msg = (radio_proto_t *) data;

    // TODO: frequency setting:
    if (!radio_frequency_done) {
        rx_cnt[radio_frequency - FREQ_MIN]++;
    }

    if (!validate(msg, len)) {
        // fail
        return;
    }

    switch(msg->msg_type) {
    case RADIO_MSG_TYPE_BOOP:
        leds_boop();

        // Also, retransmit if appropriate:
        if (msg->badge_id != badge_conf.badge_id && msg->msg_payload) {
            curr_packet_tx.proto_version = RADIO_PROTO_VER;
            curr_packet_tx.badge_id = msg->badge_id;
            curr_packet_tx.msg_type = RADIO_MSG_TYPE_BOOP;
            curr_packet_tx.msg_payload = msg->msg_payload - 1;
            crc16_append_buffer((uint8_t *)&curr_packet_tx, sizeof(radio_proto_t)-2);

            // Send the boop.
            rfm75_tx(RFM75_BROADCAST_ADDR, 1, (uint8_t *)&curr_packet_tx,
                     RFM75_PAYLOAD_SIZE);
        }
        // Fall through and also handle this as a beacon.
    case RADIO_MSG_TYPE_BEACON:
        // Handle a beacon.
        radio_handle_beacon(msg->badge_id);
        break;
    }
}

/// Send a radio message that we've done a boop.
void radio_boop(uint8_t badge_id, uint8_t seq) {
    curr_packet_tx.proto_version = RADIO_PROTO_VER;
    curr_packet_tx.badge_id = badge_id;
    curr_packet_tx.msg_type = RADIO_MSG_TYPE_BOOP;
    curr_packet_tx.msg_payload = seq;
    crc16_append_buffer((uint8_t *)&curr_packet_tx, sizeof(radio_proto_t)-2);

    // Send our boop.
    rfm75_tx(RFM75_BROADCAST_ADDR, 1, (uint8_t *)&curr_packet_tx,
             RFM75_PAYLOAD_SIZE);
}

/// Do our regular radio and queerdar interval actions.
/**
 * This function MUST NOT be called if we are in a state where the radio is
 * not allowed to initiate a transmission, because it will ALWAYS call
 * `rfm75_tx()`. That guard MUST be done outside of this function, because
 * this function has MANY side effects. Use rfm75_tx_avail() for this.
 */
void radio_interval() {
    for (uint16_t i=0; i<BADGES_IN_SYSTEM; i++) {
        if (ids_in_range[i].intervals_left) {
            // Currently in range.
            ids_in_range[i].intervals_left--;
            if (!ids_in_range[i].intervals_left) {
                // Just aged out.
                // TODO: handle aging out
                radio_badges_in_range--;
                badge_update_queerdar_count(radio_badges_in_range);
            }
        }
    }

    // Also, at each radio interval, we do need to do a beacon.
    curr_packet_tx.proto_version = RADIO_PROTO_VER;
    curr_packet_tx.badge_id = badge_conf.badge_id;
    curr_packet_tx.msg_type = RADIO_MSG_TYPE_BEACON;
    curr_packet_tx.msg_payload = 0;
    crc16_append_buffer((uint8_t *)&curr_packet_tx, sizeof(radio_proto_t)-2);

    // Send our beacon.
    rfm75_tx(RFM75_BROADCAST_ADDR, 1, (uint8_t *)&curr_packet_tx,
             RFM75_PAYLOAD_SIZE);
}

void radio_init(uint16_t addr) {
    rfm75_init(addr, &radio_rx_done, &radio_tx_done);
    rfm75_post();
    rfm75_write_reg(0x05, radio_frequency);
}
