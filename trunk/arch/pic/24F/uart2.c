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
    
    Title:          Interrupt-driven, ring buffered UART2 module
    Author(s):      Pieter Conradie
    Creation Date:  2010/04/11
    Revision Info:  $Id: $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "uart2.h"
#include "ring_buffer.h"
#include "board.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Default BAUD rate
#ifndef UART2_BAUD
#define UART2_BAUD           115200ul
#endif

/// Internal ring buffer sizes
#ifndef UART2_TX_BUFFER_SIZE
#define UART2_TX_BUFFER_SIZE 256
#endif
#ifndef UART2_RX_BUFFER_SIZE
#define UART2_RX_BUFFER_SIZE 256
#endif

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Receive ring (circular) buffer
static ring_buffer_t uart2_rx_ring_buffer;
static u8_t          uart2_rx_buffer[UART2_RX_BUFFER_SIZE];

/// Transmit ring (circular) buffer
static ring_buffer_t uart2_tx_ring_buffer;
static u8_t          uart2_tx_buffer[UART2_TX_BUFFER_SIZE];

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
// UART Receive interupt handler
void __attribute__((__interrupt__,auto_psv)) _U2RXInterrupt(void)
{
    u8_t data;

    // See if Receive FIFO buffer has data
    while(U2STAbits.URXDA == 1)
    {
        // See if Receive buffer has overflowed
        if(U2STAbits.OERR == 1)
        {
            // Reset flag
            U2STAbits.OERR = 0;
        }

        // Accept data only if there were no Framing or Parity Error(s)
        if((U2STAbits.FERR == 1) || (U2STAbits.PERR == 1))
        {
            // Received data had an error, discard received data
            data = U2RXREG;
            continue;
        }

        // Fetch data
        data = U2RXREG;

        // Buffer received data
        ring_buffer_write_byte(&uart2_rx_ring_buffer, data);
    }

    // Clear Receive interrupt flag
    IFS1bits.U2RXIF = 0;
}

// UART Transmit interrupt handler
void __attribute__((__interrupt__,auto_psv)) _U2TXInterrupt(void)
{
    u8_t data;

    // See if Transmit FIFO buffer is not full
    while(U2STAbits.UTXBF == 0)
    {
        // Fetch data to be sent from ring buffer
        if(ring_buffer_read_byte(&uart2_tx_ring_buffer, &data))
        {
            // Add data to Transmit FIFO buffer
            U2TXREG = data;
        }
        else
        {
            // No more data to be sent; disable Transmit interrupt
            IEC1bits.U2TXIE = 0;
            break;
        }
    }

    // Clear Transmit interrupt flag
    IFS1bits.U2TXIF = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void uart2_init(void)
{
    U2MODEBITS u2modebits = U2MODEbits;
    U2STABITS  u2stabits  = U2STAbits;

    // Initialise ring buffers
    ring_buffer_init(&uart2_rx_ring_buffer, uart2_rx_buffer, UART2_RX_BUFFER_SIZE);
    ring_buffer_init(&uart2_tx_ring_buffer, uart2_tx_buffer, UART2_TX_BUFFER_SIZE);

    // Set Baud Rate register
    U2BRG = DIV_ROUND(F_CY,(UART2_BAUD*16))-1;

    // Set UART2 Mode register
    u2modebits.STSEL    = 0; // One Stop bit
    u2modebits.PDSEL0   = 0;
    u2modebits.PDSEL1   = 0; // 8-bit data, no parity
    u2modebits.BRGH     = 0; // BRG generates 16 clocks per bit period (16x baud clock, Standard mode)
    u2modebits.RXINV    = 0; // UxRX Idle state is '1'
    u2modebits.ABAUD    = 0; // Baud rate measurement disabled or completed
    u2modebits.LPBACK   = 0; // Loopback mode is disabled
    u2modebits.WAKE     = 0; // No wake-up enabled
    u2modebits.UEN0     = 0;
    u2modebits.UEN1     = 0; // UxTX and UxRX pins are enabled and used; UxCTS and UxRTS/BCLKx pins controlled by PORT latches
    u2modebits.RTSMD    = 1; // UxRTS pin in Simplex mode
    u2modebits.IREN     = 0; // IrDA encoder and decoder disabled
    u2modebits.USIDL    = 0; // Continue module operation in Idle mode
    u2modebits.UARTEN   = 1; // UARTx is enabled; all UARTx pins are controlled by UARTx as defined by UEN<1:0>

    U2MODEbits = u2modebits;

    // UART2 Status and Control register
    u2stabits.ADDEN     = 0; // Address Detect mode disabled
    u2stabits.URXISEL0  = 0;
    u2stabits.URXISEL1  = 0; // Interrupt is set when any character is received and transferred from the RSR to the receive buffer. Receive buffer has one or more characters.
    u2stabits.UTXEN     = 1; // Transmit enabled, UxTX pin controlled by UARTx
    u2stabits.UTXBRK    = 0; // Sync Break transmission disabled or completed
    u2stabits.UTXINV    = 0; // UxTX Idle '1'
    u2stabits.UTXISEL0  = 0;
    u2stabits.UTXISEL1  = 1; // Interrupt when a character is transferred to the Transmit Shift Register (TSR) and as a result, the transmit buffer becomes empty

    U2STAbits = u2stabits;
                   
    // Clear UART2 Receive and Transmit interrupt flags
    IFS1bits.U2RXIF = 0; 
    IFS1bits.U2TXIF = 0; 

    // Enable UART2 Receive interrupt
    IEC1bits.U2RXIE = 1;
}

void uart2_change_baud(u32_t baud)
{
    u16_t  u2brg;
    ldiv_t div;

   /* 
    * Calculate new 16-bit U2BRG register value:
    * U2BRG = F_CY/(baud*16) - 1
    */
    baud <<= 4;
    div    = ldiv(F_CY,baud);
    u2brg  = (u16_t)div.quot;
    baud >>= 1;
    if((u32_t)(div.rem) < baud)
    {
        u2brg--;
    }
    // Set BAUD rate by initalising 16-bit U2BRG register
    U2BRG = u2brg;
}

bool_t uart2_rx_buffer_empty(void)
{
    return ring_buffer_empty(&uart2_rx_ring_buffer);
}

bool_t uart2_get_rx_byte(u8_t* data)
{
    return ring_buffer_read_byte(&uart2_rx_ring_buffer,data);
}

u16_t uart2_get_rx_data(u8_t* buffer, u16_t bytes_to_receive)
{
    return ring_buffer_read_data(&uart2_rx_ring_buffer,buffer,bytes_to_receive);
}

bool_t uart2_tx_buffer_full(void)
{
    return ring_buffer_full(&uart2_tx_ring_buffer);
}

bool_t uart2_tx_buffer_empty(void)
{
    return ring_buffer_empty(&uart2_tx_ring_buffer);
}

bool_t uart2_tx_finished(void)
{
    if(!ring_buffer_empty(&uart2_tx_ring_buffer))
    {
        return FALSE;
    }

    /* 
     * See if Transmit Shift Register is empty and transmit buffer is empty
     * (the last transmission has completed)
     */
    return (U2STAbits.TRMT ==1);
}

bool_t uart2_tx_byte(u8_t data)
{
    if(!ring_buffer_write_byte(&uart2_tx_ring_buffer,data))
    {
        // Buffer is full
        return FALSE;
    }

    // Manually enable UART2 Transmit interrupt
    IEC1bits.U2TXIE = 1;
    IFS1bits.U2TXIF = 1;

    return TRUE;
}

u16_t uart2_tx_data(const u8_t* data, u16_t bytes_to_send)
{
    u16_t bytes_sent = ring_buffer_write_data(&uart2_tx_ring_buffer, data,bytes_to_send);

    // Manually enable UART2 Transmit interrupt
    IEC1bits.U2TXIE = 1;
    IFS1bits.U2TXIF = 1;

    return bytes_sent;
}

/* _____LOG__________________________________________________________________ */
/*

 2010/04/11 : Pieter.Conradie
 - Created
   
*/

