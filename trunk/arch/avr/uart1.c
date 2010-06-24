/* =============================================================================

    Copyright (c) 2006 Pieter Conradie [www.piconomic.co.za]
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
   
    Title:          Interrupt-driven, ring buffered UART1 module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "uart1.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/** 
 *  @name Size of transmit and receive buffer
 *   
 *  @note Code will be optimized if the size is a power of two 
 *        (2,4,8,16,32,64,128 or 256).
 */
//@{
#ifndef UART1_TX_BUFFER_SIZE
#define UART1_TX_BUFFER_SIZE 64
#endif

#ifndef UART1_RX_BUFFER_SIZE
#define UART1_RX_BUFFER_SIZE 64
#endif
//@}

/* _____MACROS_______________________________________________________________ */
/// Macro to convert the specified BAUD rate to a 16-bit UBBR register value
#define UART1_UBBR_VALUE(Baud) (DIV(F_CPU,(16ul*Baud))-1)

/// Optimized macro that returns the next ring (circular) buffer index
#define UART1_NEXT_INDEX(index,buffer_size) \
    { \
        u8_t temp = index; \
        temp++; \
        /* See if buffer size is a power of two */ \
        if((buffer_size&(buffer_size-1)) == 0) \
        { \
            /* Use masking to optimize pointer wrapping to index 0 */ \
            temp &= buffer_size-1; \
        } \
        else \
        { \
            /* Wrap index to 0 if it has exceeded buffer boundary */ \
            if(temp == (u8_t)buffer_size) temp = 0; \
        } \
        index = temp; \
    }

/* _____LOCAL VARIABLES______________________________________________________ */
/// Receive ring (circular) buffer
static          u8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
static          u8_t uart1_rx_out;
static volatile u8_t uart1_rx_in;

/// Transmit ring (circular) buffer
static          u8_t uart1_tx_buffer[UART1_TX_BUFFER_SIZE];
static volatile u8_t uart1_tx_out;
static          u8_t uart1_tx_in;

/// Flag that is used by interrupt handler to indicate that transmission is finished (transmit buffer empty)
static          bool_t  uart1_tx_finished_flag;

/* _____PRIVATE FUNCTIONS____________________________________________________ */
/// Received data interrupt handler
ISR(USART1_RX_vect)
{
    u8_t ucsra = UCSR1A;
    u8_t data  = UDR1;
    u8_t index = uart1_rx_in;

    // Calculate next pointer position
    UART1_NEXT_INDEX(index, UART1_RX_BUFFER_SIZE);

    // Make sure there is space available in buffer.
    if(index == uart1_rx_out)
    {
        // Receive buffer is full, discard received data
        return;
    }
    
    // Accept data only if there were no Framing, Data Overrun or Parity Error(s)
    if(ucsra & ((1<<FE)|(1<<DOR)|(1<<UPE)))
    {
        // Received data had an error, discard received data
        return;
    }
    
    // Add data to ring buffer
    uart1_rx_buffer[uart1_rx_in] = data;
    
    // Advance pointer
    uart1_rx_in = index;
}

/// Transmit data register empty interrupt handler
ISR(USART1_UDRE_vect)
{
    // See if there is more data to be sent
    if(uart1_tx_out == uart1_tx_in)
    {
        // Disable transmit data register empty interrupt
        BIT_SET_LO(UCSR1B, UDRIE);

        // Enable transmit complete interrupt
        BIT_SET_HI(UCSR1B, TXCIE);

        return;
    }

    // Clear flag to indicate that transmission is busy
    uart1_tx_finished_flag = FALSE;

    // Send data
    UDR1 = uart1_tx_buffer[uart1_tx_out];

    // Advance pointer
    UART1_NEXT_INDEX(uart1_tx_out, UART1_TX_BUFFER_SIZE);
}

/// Transmit complete interrupt handler
ISR(USART1_TX_vect)
{
    // Set flag to indicate that transmission has finished
    uart1_tx_finished_flag = TRUE;
    
    // Disable interrupt
    BIT_SET_LO(UCSR1B,TXCIE);
}

/* _____FUNCTIONS_____________________________________________________ */
void  uart1_init(u32_t          baud, 
                 u8_t           data_bits, 
                 uart1_parity_t parity, 
                 u8_t           stop_bits)
{
    u8_t ucsrc = 0x00;

    // Initialise variables
    uart1_rx_out           = 0;
    uart1_rx_in            = 0;
    uart1_tx_out           = 0;
    uart1_tx_in            = 0;
    uart1_tx_finished_flag = TRUE;

    switch(parity)
    {
    case UART1_ODD_PARITY :
        // Odd parity
        ucsrc |= (1<<UPM11) | (1<<UPM10);
        break;
    case UART1_EVEN_PARITY :
        // Even parity
        ucsrc |= (1<<UPM11) | (0<<UPM10);
        break;
    case UART1_NO_PARITY :
        // Fall through...
    default:
        // No parity
        ucsrc |= (0<<UPM11) | (0<<UPM10);
        break;
    }

    switch(data_bits)
    {
    case 5:
        // 5 data bits
        ucsrc |= (0<<UCSZ12) | (0<<UCSZ11) | (0<<UCSZ10);
        break;
    case 6:
        // 6 data bits
        ucsrc |= (0<<UCSZ12) | (0<<UCSZ11) | (1<<UCSZ10);
        break;
    case 7:
        // 7 data bits
        ucsrc |= (0<<UCSZ12) | (1<<UCSZ11) | (0<<UCSZ10);
        break;
    case 8:
        // Fall through...
    default:
        // 8 data bits
        ucsrc |= (0<<UCSZ12) | (1<<UCSZ11) | (1<<UCSZ10);
        break;
    }

    uart1_change_baud(baud);
}

void uart1_change_baud(u32_t baud)
{
    u16_t  ubrr;
    ldiv_t div;

    // Disable UART
    UCSR1B = 0;

    // Calculate new 16-bit UBBR register value
    baud <<= 4;
    div    = ldiv(F_CPU,baud);
    ubrr   = (u16_t)div.quot;
    baud >>= 1;
    if((u32_t)(div.rem) < baud)
    {
        ubrr--;
    }
    // Set BAUD rate by initalising 16-bit UBBR register
    UBRR1H = U16_HI8(ubrr);
    UBRR1L = U16_LO8(ubrr);

    // Enable RxD/TxD and receive interupt
    UCSR1B = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
}

bool_t uart1_rx_buffer_empty(void)
{
    // See if there is data in the buffer
    if(uart1_rx_out == uart1_rx_in)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


bool_t uart1_get_rx_byte(u8_t* data)
{
    // See if there is data in the buffer
    if(uart1_rx_out == uart1_rx_in)
    {
        return FALSE;
    }
    // Fetch data
    *data = uart1_rx_buffer[uart1_rx_out];

    // Advance pointer
    UART1_NEXT_INDEX(uart1_rx_out, UART1_RX_BUFFER_SIZE);

    return TRUE;
}

u8_t uart1_get_rx_data(u8_t* buffer, u8_t max_buf_size)
{
    u8_t data_received = 0;

    while(max_buf_size)
    {
        // See if there is data in the buffer
        if(uart1_rx_out == uart1_rx_in)
        {
            break;
        }
        // Fetch data
        *buffer++ = uart1_rx_buffer[uart1_rx_out];

        // Advance pointer
        UART1_NEXT_INDEX(uart1_rx_out, UART1_RX_BUFFER_SIZE);

        // Next byte
        data_received++;
        max_buf_size--;
    }

    return data_received;
}

bool_t uart1_tx_buffer_full(void)
{
    u8_t index = uart1_tx_in;
    
    // Calculate next pointer position
    UART1_NEXT_INDEX(index, UART1_TX_BUFFER_SIZE);

    if(index == uart1_tx_out)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool_t uart1_tx_buffer_empty(void)
{
    if(uart1_tx_out == uart1_tx_in)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool_t uart1_tx_finished(void)
{
    if(uart1_tx_out != uart1_tx_in)
    {
        return FALSE;
    }
    return uart1_tx_finished_flag;
}

bool_t uart1_tx_byte(u8_t data)
{
    u8_t index = uart1_tx_in;
    
    // Calculate next pointer position
    UART1_NEXT_INDEX(index, UART1_TX_BUFFER_SIZE);

    // Make sure there is space available in buffer
    if(index == uart1_tx_out)
    {
        return FALSE;
    }

    // Insert data into buffer
    uart1_tx_buffer[uart1_tx_in] = data;

    // Advance pointer
    uart1_tx_in = index;

    // Make sure transmit process is started by enabling interrupt
    BIT_SET_HI(UCSR1B, UDRIE);

    return TRUE;
}

u8_t uart1_tx_data(const u8_t* data, u8_t bytes_to_send)
{
    u8_t bytes_buffered = 0;
    u8_t index;

    while(bytes_to_send)
    {
        // Calculate next pointer position
        index = uart1_tx_in;
        UART1_NEXT_INDEX(index, UART1_TX_BUFFER_SIZE);

        // Make sure there is space available in buffer
        if(index == uart1_tx_out)
        {
            break;
        }

        // Insert data into buffer
        uart1_tx_buffer[uart1_tx_in] = *data++;

        // Advance pointer
        uart1_tx_in = index;

        // Next byte
        bytes_buffered++;
        bytes_to_send--;
    }

    // Make sure transmit process is started by enabling interrupt
    BIT_SET_HI(UCSR1B, UDRIE);

    return bytes_buffered;
}

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : Pieter Conradie
 - First release
   
*/
