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
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "uart_poll.h"

/* _____MACROS_______________________________________________________________ */
/// Macro to convert the specified BAUD rate to a 16-bit UBBR register value
#define UART_UBBR_VAL(Baud) (DIV_ROUND(F_CPU,(Baud<<4))-1)

/* ___LOCAL DEFINITIONS___________________________________________________ */
#if (UART_PORT_SELECT==0)

#define UCSRA UCSR0A
#define UCSRB UCSR0B
#define UCSRC UCSR0C
#define UBRRH UBRR0H
#define UBRRL UBRR0L
#define UDR   UDR0

#elif (UART_PORT_SELECT==1)

#define UCSRA UCSR1A
#define UCSRB UCSR1B
#define UCSRC UCSR1C
#define UBRRH UBRR1H
#define UBRRL UBRR1L
#define UDR   UDR1

#else
#error "UART_PORT_SELECT not correct!"
#endif

/* _____FUNCTIONS_____________________________________________________ */
void uart_poll_init(void)
{
    // Enable RxD/TxD
    UCSRB = (1<<RXEN)|(1<<TXEN);

    // Set baud rate
    UBRRH = U16_HI8(UART_UBBR_VAL(UART_BAUD));
    UBRRL = U16_LO8(UART_UBBR_VAL(UART_BAUD));

    // Set frame format to 8 data bits, 1 stop bit
    UCSRC = (0<<USBS)|(1<<UCSZ1)|(1<<UCSZ0);
}

bool_t uart_poll_tx_byte(u8_t data)
{
    // Wait for transmit register to be empty
    LOOP_UNTIL_BIT_IS_HI(UCSRA,UDRE);

    // Send data
    UDR = data;

    return TRUE;
}

bool_t uart_poll_rx_byte(u8_t* data)
{
    // See if data received flag is set
    if(BIT_IS_LO(UCSRA,RXC))
    {
        return FALSE;
    }

    // Get received data from buffer
    *data = UDR;

    return TRUE;
}

bool_t uart_poll_rx_data_available(void)
{
    // See if data received flag is set
    if(BIT_IS_HI(UCSRA,RXC))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 *  @}
 */

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : PJC
 - First release
   
*/
