#ifndef __SPI1_H__
#define __SPI1_H__
/* =============================================================================

    Copyright (c) 2010 Pieter Conradie [www.piconomic.co.za]
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
    
    * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
    
    * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
    
    Title:          SPI1 Driver
    Author(s):      Pieter Conradie
    Creation Date:  2010-04-13
    Revision Info:  $Id: $

============================================================================= */

/** 
 *  @ingroup PIC24F
 *  @defgroup SPI1 spi1.h : SPI1 Driver
 *
 *  Driver for the SPI1 peripheral to communicate with SPI slaves.
 *
 *  Files: pic\24F\spi1.h & pic\24F\spi1.c
 *
 *  The SPI peripheral is initialised with spi1_init(). The SPI port pins are
 *  initialised for MASTER mode of operation. The data order is Most Significant
 *  Bit first (MSB order: D7, D6, ..., D0). Output data bit value (MOSI pin) is 
 *  set on the rising edge of the clock and input data bit value (MISO pin) is 
 *  sampled on the falling edge of the clock. The clock bit rate is set to 
 *  F_CY / 32.
 * 
 *  No automatic mechanism is provided to select a slave device on the SPI bus.
 *  The slave device's chip select (CS) must be manually driven low at the start 
 *  of the transfer and driven high after the transfer is complete.
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise SPI peripheral.
 * 
 *  - SPI peripheral settings:
 *      - Master mode (as opposed to slave mode)
 *      - Data order is Most Significant Bit first (MSB: D7, D6, ..., D0)
 *      - Mode 0 : CPOL=0, CPHA=0; Set data on rising edge, sample data on falling edge
 *      - SCK Bit rate is F_CY / 32
 * 
 */
extern void spi1_init(void);

/**
 *  Send 8 bits of data over SPI
 * 
 *  @param[in]  tx_data    8-bit data to send
 */
extern void spi1_tx_byte(u8_t tx_data);

/**
 *  Receive 8 bits of data over SPI
 * 
 *  @returns    u8_t       8-bit received data
 */
extern u8_t spi1_rx_byte(void);

/**
 *  Transfer 8 bits of data over SPI (send and receive).
 * 
 *  @param[in]  tx_data    8-bit data to send
 *  @returns    u8_t       8-bit received data
 */
extern u8_t spi1_transfer_byte(u8_t tx_data);

/** 
 * Transmits a number of bytes over SPI.
 *  
 * @param tx_data           Pointer to a buffer containing data to be transmitted
 * @param bytes_to_transmit Number of bytes to send
 */
extern void spi1_tx_data(const void *tx_data, size_t bytes_to_transmit);

/** 
 * Receives a number of bytes over SPI.
 *  
 * @param rx_data           Pointer to a buffer where the received data must be stored
 * @param bytes_to_receive  Number of bytes to receive
 */
extern void spi1_rx_data(void *rx_data, size_t bytes_to_receive);

/** 
 * Transfers a number of bytes over SPI (send and receive). 
 *  
 * The transmit buffer and receive buffer must both be at least 
 * "bytes_to_transfer" bytes in size. 
 *  
 * The transmit buffer and receive buffer may be the same buffer, which means 
 * that the transmitted data will be overwritten by the received data. 
 * 
 * @param tx_data           Pointer to a buffer containing data to be transmitted
 * @param rx_data           Pointer to a buffer where the received data must be stored
 * @param bytes_to_transfer Number of bytes to transfer
 */
extern void spi1_transfer_data(const void *tx_data, void *rx_data, size_t bytes_to_transfer);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */

#endif
