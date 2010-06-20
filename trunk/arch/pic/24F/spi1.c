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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "spi1.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void spi1_init(void)
{
    SPI1CON1BITS spi1con1bits;
    SPI1CON2BITS spi1con2bits;
    SPI1STATBITS spi1statbits;

    spi1con1bits.PPRE0  = 1;
    spi1con1bits.PPRE1  = 0; /* Primary prescale 16:1 */
    spi1con1bits.SPRE   = 6; /* Secondary prescale 2:1 => 500kHz bit rate */
    spi1con1bits.MSTEN  = 1; /* Master mode */
    spi1con1bits.CKP    = 0; /* Idle state for clock is a low level; active state is a high level */
    spi1con1bits.SSEN   = 0; /* SSx pin not used by module; pin controlled by port function */
    spi1con1bits.CKE    = 1; /* Serial output data changes on transition from active clock state to Idle clock state */
    spi1con1bits.SMP    = 0; /* Input data sampled at middle of data output time */
    spi1con1bits.MODE16 = 0; /* Communication is byte-wide (8 bits) */
    spi1con1bits.DISSDO = 0; /* SDOx pin is controlled by the module */
    spi1con1bits.DISSCK = 0; /* Internal SPI clock is enabled */

    SPI1CON1bits = spi1con1bits;

    
    spi1con2bits = SPI1CON2bits;

    spi1con2bits.SPIBEN = 0; /* Enhanced Buffer disabled (Legacy mode) */
    spi1con2bits.SPIFE  = 0; /* Frame sync pulse precedes first bit clock */
    spi1con2bits.SPIFPOL= 0; /* Frame sync pulse is active-low */
    spi1con2bits.SPIFSD = 0; /* Frame sync pulse output (master) */
    spi1con2bits.FRMEN  = 0; /* Framed SPIx support disabled */

    SPI1CON2bits = spi1con2bits;

    spi1statbits.SISEL  = 0; /* Interrupt when the last data in the receive buffer is read; as a result, the buffer is empty */
    spi1statbits.SPIROV = 0; /* Clear Receive Overflow Flag bit */
    spi1statbits.SPISIDL= 0; /* Continues module operation in Idle mode */
    spi1statbits.SPIEN  = 1; /* Enables module and configures SCKx, SDOx, SDIx and SSx as serial port pins */

    SPI1STATbits = spi1statbits;
}

void spi1_tx_byte(u8_t tx_data)
{
    u8_t rx_data;

    // Write data to be transmitted
    SPI1BUF = tx_data;

    /* 
     * Wait until Receive Buffer Full Status bit is set, indicating that
     * receive is complete, SPI1RXB is full.
     */
    while(SPI1STATbits.SPIRBF == 0)
    {
        ;
    }

    // Read and discard data
    rx_data = SPI1BUF;
}

u8_t spi1_rx_byte(void)
{
    u8_t rx_data;

    // Write dummy data to be transmitted
    SPI1BUF = 0x00;

    /* 
     * Wait until Receive Buffer Full Status bit is set, indicating that
     * receive is complete, SPI1RXB is full.
     */
    while(SPI1STATbits.SPIRBF == 0)
    {
        ;
    }

    // Read data
    rx_data = SPI1BUF;

    return rx_data;
}

u8_t spi1_transfer_byte(u8_t tx_data)
{
    u8_t rx_data;

    // Write data to be transmitted
    SPI1BUF = tx_data;

    /* 
     * Wait until Receive Buffer Full Status bit is set, indicating that
     * receive is complete, SPI1RXB is full.
     */
    while(SPI1STATbits.SPIRBF == 0)
    {
        ;
    }

    // Read data
    rx_data = SPI1BUF;

    return rx_data;
}

void spi1_tx_data(const void *tx_data, size_t bytes_to_transmit)
{
    u8_t data;
    const u8_t *tx_data_byte = (u8_t *)tx_data;

    while(bytes_to_transmit)
    {
        // Write data to be transmitted
        SPI1BUF = *tx_data_byte++;
    
        /* 
         * Wait until Receive Buffer Full Status bit is set, indicating that
         * receive is complete, SPI1RXB is full.
         */
        while(SPI1STATbits.SPIRBF == 0)
        {
            ;
        }
    
        // Read data
        data = SPI1BUF;

        // Next byte
        bytes_to_transmit--;
    }
}

void spi1_rx_data(void *rx_data, size_t bytes_to_receive)
{
    u8_t data;
    u8_t *rx_data_byte = (u8_t *)rx_data;

    while(bytes_to_receive)
    {
        // Write dummy data to be transmitted
        SPI1BUF = 0x00;
    
        /* 
         * Wait until Receive Buffer Full Status bit is set, indicating that
         * receive is complete, SPI1RXB is full.
         */
        while(SPI1STATbits.SPIRBF == 0)
        {
            ;
        }
    
        // Read data
        data = SPI1BUF;

        // Buffer received data
        *rx_data_byte++ = data;

        // Next byte
        bytes_to_receive--;
    }
}

void spi1_transfer_data(const void *tx_data, void *rx_data, size_t bytes_to_transfer)
{
    u8_t data;
    const u8_t *tx_data_byte = (u8_t *)tx_data;
    u8_t *rx_data_byte       = (u8_t *)rx_data;

    while(bytes_to_transfer)
    {
        // Write data to be transmitted
        SPI1BUF = *tx_data_byte++;
    
        /* 
         * Wait until Receive Buffer Full Status bit is set, indicating that
         * receive is complete, SPI1RXB is full.
         */
        while(SPI1STATbits.SPIRBF == 0)
        {
            ;
        }
    
        // Read data
        data = SPI1BUF;

        // Buffer received data
        *rx_data_byte++ = data;

        // Next byte
        bytes_to_transfer--;
    }
}

/* _____LOG__________________________________________________________________ */
/*

 2010-04-13 : Pieter.Conradie
 - First release
   
*/
