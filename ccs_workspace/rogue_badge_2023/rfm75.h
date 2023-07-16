/// Low-level driver headers for HopeRF RFM75 module.
/**
 **
 **
 ** \file rfm75.h
 ** \author George Louthan
 ** \date   2023
 ** \copyright (c) 2018-2023 George Louthan @duplico. MIT License.
 */

#ifndef RFM75_H_
#define RFM75_H_

#include <stdint.h>
#include <msp430.h>

#define RFM75_PAYLOAD_SIZE 22
#define UNICAST_LSB 0
#define BROADCAST_LSB 0xEE
#define RFM75_BROADCAST_ADDR 0xffff

// Pin and peripheral configurations:

#ifndef RFM75_OVERRIDE_DEFAULTS

#define RFM75_UCxIFG UCB0IFG
#define RFM75_UCxTXBUF UCB0TXBUF
#define RFM75_UCxRXBUF UCB0RXBUF
#define RFM75_UCxCTLW0 UCB0CTLW0
#define RFM75_UCxBRW UCB0BRW

#define RFM75_CSN_OUT P1OUT
#define RFM75_CSN_DIR P1DIR
#define RFM75_CSN_PIN GPIO_PIN0
#define RFM75_CE_OUT P1OUT
#define RFM75_CE_DIR P1DIR
#define RFM75_CE_PIN BIT6

#define RFM75_USCI_PORT GPIO_PORT_P1
#define RFM75_USCI_PINS (GPIO_PIN1+GPIO_PIN2+GPIO_PIN3)

#define RFM75_IRQ_DIR P1DIR
#define RFM75_IRQ_REN P1REN
#define RFM75_IRQ_SEL0 P1SEL0
#define RFM75_IRQ_SEL1 P1SEL1
#define RFM75_IRQ_IES P1IES
#define RFM75_IRQ_IFG P1IFG
#define RFM75_IRQ_IE P1IE
#define RFM75_IRQ_PIN BIT7

#define RFMISR_VECTOR PORT1_VECTOR
#define RFMxIV P1IV
#define RFMxIV_PxIFGx P1IV_P1IFG7

#endif

//************************FSK COMMAND and REGISTER****************************************//
// SPI(RFM75) commands
#define READ_REG                0x00  // Define read command to register
#define WRITE_REG               0x20  // Define write command to register
#define RD_RX_PLOAD             0x61  // Define RX payload register address
#define WR_TX_PLOAD             0xA0  // Define TX payload register address
#define FLUSH_TX                0xE1  // Define flush TX register command
#define FLUSH_RX                0xE2  // Define flush RX register command
#define REUSE_TX_PL             0xE3  // Define reuse TX payload register command
#define WR_TX_PLOAD_NOACK       0xb0
#define W_ACK_PAYLOAD_CMD       0xa8
#define ACTIVATE_CMD            0x50
#define R_RX_PL_WID_CMD         0x60
#define NOP_NOP                 0xFF  // Define No Operation, might be used to read status register

// SPI(RFM75) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define PAYLOAD_WIDTH   0x1f  // 'payload length of 256 bytes modes register address
#define FEATURE         0x1d

#define CONFIG_MASK_RX_DR BIT6
#define CONFIG_MASK_TX_DS BIT5
#define CONFIG_MASK_MAX_RT BIT4
#define CONFIG_EN_CRC BIT3
#define CONFIG_CRCO_1BYTE 0x00
#define CONFIG_CRCO_2BYTE BIT2
#define CONFIG_PWR_UP BIT1
#define CONFIG_PRIM_RX BIT0
#define CONFIG_PRIM_TX 0x00

//interrupt status
#define STATUS_RX_DR    0x40
#define STATUS_TX_DS    0x20
#define STATUS_MAX_RT   0x10

#define STATUS_TX_FULL  0x01

//FIFO_STATUS
#define FIFO_STATUS_TX_REUSE    0x40
#define FIFO_STATUS_TX_FULL     0x20
#define FIFO_STATUS_TX_EMPTY    0x10

#define FIFO_STATUS_RX_FULL     0x02
#define FIFO_STATUS_RX_EMPTY    0x01

#define RFM75_CRC_SEED 0x31C0

// State values:
#define RFM75_BOOT 0
#define RFM75_RX_INIT 1
#define RFM75_RX_LISTEN 2
#define RFM75_RX_READY 3
#define RFM75_TX_INIT 4
#define RFM75_TX_READY 5
#define RFM75_TX_FIFO 6
#define RFM75_TX_SEND 7
#define RFM75_TX_DONE 8

typedef void rfm75_rx_callback_fn(uint8_t* data, uint8_t len, uint8_t pipe);
typedef void rfm75_tx_callback_fn(uint8_t ack);

void rfm75_init(uint16_t unicast_address, rfm75_rx_callback_fn *rx_callback, rfm75_tx_callback_fn *tx_callback);
uint8_t rfm75_post();
void rfm75_deferred_interrupt();
uint8_t rfm75_tx_avail();
void rfm75_tx(uint16_t addr, uint8_t noack, uint8_t* data, uint8_t len);
void rfm75_write_reg(uint8_t reg, uint8_t data);

extern uint32_t rfm75_seqnum;
extern volatile uint8_t f_rfm75_interrupt;

#endif /* RFM75_H_ */
