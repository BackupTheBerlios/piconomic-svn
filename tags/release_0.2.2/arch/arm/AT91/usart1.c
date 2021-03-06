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
    
    Title:          Interrupt-driven, ring buffered USART1 module
    Author(s):      Pieter Conradie
    Creation Date:  2008/08/06
    Revision Info:  $Id: usart1.c 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <usart/usart.h>
#include <pio/pio.h>
#include <aic/aic.h>
#include <pmc/pmc.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "usart1.h"
#include "ring_buffer.h"
#include "board.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Receive ring (circular) buffer
static ring_buffer_t usart1_rx_ring_buffer;
static u8_t          usart1_rx_buffer[USART1_RX_BUFFER_SIZE];

/// Transmit ring (circular) buffer
static ring_buffer_t usart1_tx_ring_buffer;
static u8_t          usart1_tx_buffer[USART1_TX_BUFFER_SIZE];

/// Flag that is used by interrupt handler to indicate that transmission is finished (transmit buffer empty)
static bool_t        usart1_tx_finished_flag;

/// List of pins that must be configured for use by the application.
static const Pin USART1_Pins[] = {PIN_USART1_TXD, PIN_USART1_RXD};

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void usart1_interrupt(void)
{
    u8_t data;

    // Read status register
    u32_t csr = AT91C_BASE_US1->US_CSR;

    // See if a receive error was detected
    if(csr & (AT91C_US_RXBRK | AT91C_US_OVRE | AT91C_US_FRAME | AT91C_US_PARE))
    {
        // Reset status bits
        AT91C_BASE_US1->US_CR = AT91C_US_RSTSTA;
        return;
    }

    // See if a character has been received
    if(csr & AT91C_US_RXRDY)
    {
        // Read character and buffer it
        data = (char)(AT91C_BASE_US1->US_RHR);
        ring_buffer_write_byte(&usart1_rx_ring_buffer, data);
    }

    // See if the transmitter is ready
    if(csr & AT91C_US_TXRDY)
    {
        // See if there is more data to be sent
        if(ring_buffer_read_byte(&usart1_tx_ring_buffer,&data))
        {
            // Clear flag to indicate that transmission is busy
            usart1_tx_finished_flag = FALSE;
            // Buffer data to be transmitted
            AT91C_BASE_US1->US_THR = data;            
        }
        else
        {
            // Disable transmit ready interrupt
            AT91C_BASE_US1->US_IDR = AT91C_US_TXRDY;
            // Enable transmit empty interrupt
            AT91C_BASE_US1->US_IER = AT91C_US_TXEMPTY;
        }
    }

    // See if the transmitter is finished
    if(csr & AT91C_US_TXEMPTY)
    {
        // Set flag to indicate that transmission is finished
        usart1_tx_finished_flag = TRUE;
        // Disable transmit empty interrupt
        AT91C_BASE_US1->US_IDR = AT91C_US_TXEMPTY;
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void usart1_init(void)
{
    // Clear flag
    usart1_tx_finished_flag = FALSE;

    // Initialise ring buffers
    ring_buffer_init(&usart1_rx_ring_buffer, usart1_rx_buffer, USART1_RX_BUFFER_SIZE);
    ring_buffer_init(&usart1_tx_ring_buffer, usart1_tx_buffer, USART1_TX_BUFFER_SIZE);

    // Configure PIO pins for USART1 peripheral
    PIO_Configure(USART1_Pins, PIO_LISTSIZE(USART1_Pins));    

    // Disable the interrupt on the interrupt controller
    AIC_DisableIT(AT91C_ID_US1);

    // Enable USART1 clock
    PMC_EnablePeripheral(AT91C_ID_US1);

    // Disable all USART1 interrupts
    AT91C_BASE_US1->US_IDR = 0xFFFFFFFF;

    // Configure USART
    USART_Configure(AT91C_BASE_US1,
                    AT91C_US_USMODE_NORMAL |
                    AT91C_US_CLKS_CLOCK    |
                    AT91C_US_CHRL_8_BITS   | 
                    AT91C_US_PAR_NONE      | 
                    AT91C_US_NBSTOP_1_BIT,
                    115200,
                    BOARD_MCK);

    USART_SetTransmitterEnabled(AT91C_BASE_US1, 1);
    USART_SetReceiverEnabled(AT91C_BASE_US1, 1);

    // Configure the AIC for USART1 interrupts
    AIC_ConfigureIT(AT91C_ID_US1, AT91C_AIC_PRIOR_LOWEST | AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, usart1_interrupt);

    // Enable the interrupt on the interrupt controller
    AIC_EnableIT(AT91C_ID_US1);

    // Enable selected USART1 interrupts
    AT91C_BASE_US1->US_IER = AT91C_US_RXRDY     |
                             AT91C_US_RXBRK     |
                             AT91C_US_OVRE      |
                             AT91C_US_FRAME     |
                             AT91C_US_PARE;
}

void usart1_change_baud(u32_t baud)
{
    USART_SetTransmitterEnabled(AT91C_BASE_US1, 0);
    USART_SetReceiverEnabled(AT91C_BASE_US1, 0);

    AT91C_BASE_US1->US_BRGR = (BOARD_MCK / baud) / 16;

    USART_SetTransmitterEnabled(AT91C_BASE_US1, 1);
    USART_SetReceiverEnabled(AT91C_BASE_US1, 1);
}

bool_t usart1_rx_buffer_empty(void)
{
    return ring_buffer_empty(&usart1_rx_ring_buffer);
}

bool_t usart1_get_rx_byte(u8_t* data)
{
    return ring_buffer_read_byte(&usart1_rx_ring_buffer,data);
}

u16_t usart1_get_rx_data(u8_t* buffer, u16_t bytes_to_receive)
{
    return ring_buffer_read_data(&usart1_rx_ring_buffer,buffer,bytes_to_receive);
}

bool_t usart1_tx_buffer_full(void)
{
    return ring_buffer_full(&usart1_tx_ring_buffer);
}

bool_t usart1_tx_buffer_empty(void)
{
    return ring_buffer_empty(&usart1_tx_ring_buffer);
}

bool_t usart1_tx_finished(void)
{
    if(!ring_buffer_empty(&usart1_tx_ring_buffer))
    {
        return FALSE;
    }

    return usart1_tx_finished_flag;
}

bool_t usart1_tx_byte(u8_t data)
{
    if(!ring_buffer_write_byte(&usart1_tx_ring_buffer,data))
    {
        // Buffer is full
        return FALSE;
    }

    // Make sure transmission has started
    AT91C_BASE_US1->US_IER = AT91C_US_TXRDY;

    return TRUE;
}

u16_t usart1_tx_data(const u8_t* data, u16_t bytes_to_send)
{
    u16_t u16BytesSent = ring_buffer_write_data(&usart1_tx_ring_buffer,data,bytes_to_send);

    // Make sure transmission is started
    AT91C_BASE_US1->US_IER = AT91C_US_TXRDY;

    return u16BytesSent;
}

/* _____LOG__________________________________________________________________ */
/*

 2008/08/06 : Pieter.Conradie
 - Created
   
*/

