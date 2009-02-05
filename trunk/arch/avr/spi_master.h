#ifndef __SPI_MASTER_H__
#define __SPI_MASTER_H__
/* =============================================================================

    Copyright (c) 2006 Pieter Conradie <pieterconradie@users.berlios.de>
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
    
    Title:          Simple polled SPI Master
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: spi_master.h 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup SPI_MASTER spi_master.h : Simple polled SPI Master
 *
 *  Driver for the SPI peripheral to communicate with SPI slaves.
 *
 *  Files: spi_master.h & spi_master.c
 *
 *  The SPI peripheral is initialised with SPI_vInit(). The SPI port pins are
 *  initialised for MASTER mode of operation. The data order is Most Significant
 *  Bit first (MSB order: D7, D6, ..., D0). Output data bit value (MOSI pin) is 
 *  set on the rising edge of the clock and input data bit value (MISO pin) is 
 *  sampled on the falling edge of the clock. The clock bit rate is set to 
 *  F_CPU / 4 (230,400 bytes/s for 7.3728 MHz crystal).
 * 
 *  No automatic mechanism is provided to select a slave device on the SPI bus.
 *  The slave device's chip select (CS) must be manually driven low at the start 
 *  of the transfer and driven high after the transfer is complete.
 * 
 *  This driver does not use the interrupt mechanism, because of the fast transfer
 *  speed of the SPI peripheral. At an SPI clock bit rate of F_CPU / 4, an
 *  SPI transfer complete interrupt would be generated 32 clock cycles after 
 *  the start of each 8-bit transfer and the interrupt handler would not be able 
 *  to service the interrupt fast enough for continuous transfer or leave room for 
 *  other processing.
 * 
 *  @note The SPI peripheral Slave Select pin (SS) must be configured as an output
 *        or kept high if it is configured as an input. If this pin is configured
 *        as an input and pulled low, the SPI peripheral will automatically change 
 *        from MASTER mode to SLAVE mode. This feature is included for multi-master
 *        type configurations/topologies. This driver configures the pin as an input 
 *        with pull-up enabled.
 *  
 *  @par Example: 
 *  Extract from @ref AT45DB041B
 *  @dontinclude sflash_AT45DB041B.c
 *  @skip u8_t AT45DB041B_u8GetStatus(void)
 *  @until } // Doxygen end of example
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise SPI peripheral.
 * 
 *  - SPI IO pins initialisation:
 *      - SS (input with pull-up enabled)
 *      - SCK (output)
 *      - MOSI (output)
 *      - MISO (input) 
 *  - SPI peripheral settings:
 *      - Master mode (as opposed to slave mode)
 *      - Data order is Most Significant Bit first (MSB: D7, D6, ..., D0)
 *      - Mode 0 : CPOL=0, CPHA=0; Set data on rising edge, sample data on falling edge
 *      - SCK Bit rate is F_CPU / 4 (230,400 bytes/s for 7.3728 MHz crystal)
 * 
 */
extern void spi_init(void);

/**
 *  Send 8 bits of data over SPI
 * 
 *  @param[in]  tx_data    8-bit data to send
 */
extern void spi_tx_byte(u8_t tx_data);

/**
 *  Receive 8 bits of data over SPI
 * 
 *  @returns    u8_t       8-bit received data
 */
extern u8_t spi_rx_byte(void);

/**
 *  Transfer 8 bits of data over SPI (send and receive).
 * 
 *  @param[in]  tx_data    8-bit data to send
 *  @returns    u8_t       8-bit received data
 */
extern u8_t spi_transfer_byte(u8_t tx_data);

/** 
 * Transmits a number of bytes over SPI.
 *  
 * @param tx_data           Pointer to a buffer containing data to be transmitted
 * @param bytes_to_transmit Number of bytes to send
 */
extern void spi_tx_data(u8_t *tx_data, size_t bytes_to_transmit);

/** 
 * Receives a number of bytes over SPI.
 *  
 * @param rx_data           Pointer to a buffer where the received data must be stored
 * @param bytes_to_receive  Number of bytes to receive
 */
extern void spi_rx_data(u8_t *rx_data, size_t bytes_to_receive);

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
extern void spi_transfer_data(u8_t *tx_data, u8_t *rx_data, size_t bytes_to_transfer);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */

#endif
