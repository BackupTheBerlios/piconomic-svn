#ifndef __USART0_H__
#define __USART0_H__
/* =============================================================================

    Copyright (c) 2008 Pieter Conradie <pieterconradie@users.berlios.de>
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
    
    Title:          Interrupt-driven, ring buffered USART0 module
    Author(s):      Pieter Conradie
    Creation Date:  2008/08/06
    Revision Info:  $Id: usart0.h 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/** 
 *  @ingroup AT91
 *  @defgroup AT91_USART0 usart0.h : Interrupt-driven, ring buffered USART0
 *            module
 *
 *  Driver that initialises USART0 and allows buffered communication.
 *
 *  Files: usart0.h & usart0.c
 *
 *  This is a driver for the USART0 peripheral that provides buffered serial 
 *  communication facilities. It uses two ring (circular) buffers for 
 *  data to be sent and data received. If the receive buffer is
 *  full, extra received data is discarded. If more transmit data is
 *  specified than can be buffered, then only the number that can be buffered
 *  is accepted and the rest ignored.
 *
 *  The size of the receive and transmit buffer can be set separately. The
 *  default frame format is 8 data bits, no parity, 1 stop bit, but this can
 *  easily be changed in usart0_init().
 *
 *  @note The maximum number of bytes stored is one less than buffer size.
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */
/// Default BAUD rate
#ifndef USART0_BAUD
#define USART0_BAUD           115200ul
#endif

/// Internal ring buffer sizes
#ifndef USART0_TX_BUFFER_SIZE
#define USART0_TX_BUFFER_SIZE 256
#endif
#ifndef USART0_RX_BUFFER_SIZE
#define USART0_RX_BUFFER_SIZE 256
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise USART0 peripheral, buffers and interrupt handlers.
 * 
 */
extern void usart0_init(void);

/**
 *  Change to new BAUD rate.
 *  
 *  @param[in] baud  New BAUD rate
 */
extern void usart0_change_baud(u32_t baud);

/**
 *  See if there is received data in the receive buffer.
 * 
 *  @retval TRUE    There is received data in the receive buffer
 *  @retval FALSE   The receive buffer is empty
 */
extern bool_t usart0_rx_buffer_empty(void);

/**
 *  See if received byte is available and store it in specified location.
 * 
 *  @param[out] data Pointer to location where data byte must be stored
 * 
 *  @retval TRUE  Received byte is stored in specified location
 *  @retval FALSE No received data available (receive buffer empty)
 */
extern bool_t usart0_get_rx_byte(u8_t *data);

/**
 *  Copy received data from ring buffer into specified buffer.
 * 
 *  @param[out] buffer          Buffer to copy received data into
 *  @param[in] bytes_to_receive   Maximum number of received bytes to copy into
 *                                 buffer
 * 
 *  @return u16_t Number of received bytes copied into buffer
 */
extern u16_t  usart0_get_rx_data(u8_t *buffer, 
                                  u16_t bytes_to_receive);

/** 
 *  See if transmit ring buffer can accept more data.
 * 
 *  @retval TRUE  Transmit ring buffer is full
 *  @retval FALSE Transmit ring buffer has space for at least one byte
 */
extern bool_t usart0_tx_buffer_full(void);

/** 
 *  See if transmit ring buffer is empty.
 * 
 *  @note Buffer may be empty, but UART peripheral may still be busy 
 *        with the transmission of the last byte in the buffer. 
 *        @see usart0_tx_finished.
 * 
 *  @retval TRUE  Transmit ring buffer is empty
 *  @retval FALSE Transmit ring buffer has space for at least one byte
 */
extern bool_t usart0_tx_buffer_empty(void);

/** 
 *  See if all transmission has finished, including last byte.
 * 
 *  This functions is useful for communication standards like RS-485 where the
 *  mode must be changed manually from TX to RX after transmission.
 * 
 *  @retval TRUE  Transmision completely finished
 *  @retval FALSE Busy with transmission
 */
extern bool_t usart0_tx_finished(void);

/**
 *  Buffer one byte for transmission.
 * 
 *  @param[in] data   Byte to be transmitted
 * 
 *  @retval TRUE    Byte has been buffered
 *  @retval FALSE   Byte has not been buffered, because transmit buffer is full
 */
extern bool_t usart0_tx_byte(u8_t data);

/**
 *  Buffer byte(s) for transmission.
 * 
 *  @note The transmit ring buffer may not be able to hold all of the specified
 *        data.
 * 
 *  @param[in] data             Buffer containing data for transmission
 *  @param[in] bytes_to_send    Number of bytes in buffer to be transmitted
 * 
 *  @return u8_t                The actual number of bytes buffered for transmission.
 */
extern u16_t usart0_tx_data(const u8_t *data, 
                            u16_t      bytes_to_send);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
