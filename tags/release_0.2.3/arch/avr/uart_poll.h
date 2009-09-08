#ifndef __UART_POLL_H__
#define __UART_POLL_H__
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
    
    Title:          Basic polled UART module with no buffering
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: uart_poll.h 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup UART_POLL uart_poll.h : Basic polled UART module with no
 *            buffering
 *
 *  Simple polled serial communication facilities over UART0 or UART1.
 *
 *  Files: uart_poll.h & uart_poll.c
 *
 *  @par Example:
 *  @include uart_poll_test.c
 * 
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Default BAUD rate
#ifndef UART_BAUD
#define UART_BAUD 115200ul
#endif

/// UART/USART Port selection
#ifndef UART_PORT_SELECT
#define UART_PORT_SELECT 0
#endif

/* _____FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise UART peripheral.
 * 
 */
extern void uart_poll_init(void);

/**
 *  Wait until transmit buffer is empty and then buffer one byte for
 *  transmission.
 * 
 *  @param data   Byte to be sent
 *  
 *  @retval TRUE  Byte has been inserted into transmit buffer
 * 
 */
extern bool_t uart_poll_tx_byte(u8_t data);

/** 
 * See if a byte has been received and retrieve it.
 * 
 * @param data      Storage location for received byte
 * 
 * @retval TRUE     Byte has been succesfully received
 * @retval FALSE    No byte has been received
 *  
 */
extern bool_t uart_poll_rx_byte(u8_t* data);

/**
 *  See if a byte has been received.
 * 
 *  @retval TRUE  Byte has been received
 *  @retval FALSE Receive register empty
 */
extern bool_t uart_poll_rx_data_available(void);


/**
 *  @}
 */

#endif
