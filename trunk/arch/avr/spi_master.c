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
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "spi_master.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Define default value for SPI Control Register (overridable)
#ifndef SPI_SPCR
#define SPI_SPCR  (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0)
#endif

// Define default value for SPI Status Register (overridable)
#ifndef SPI_SPSR
#define SPI_SPSR  (0<<SPI2X)
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
#warning "There is a caveat with the SS pin when using the SPI in master mode!. See documentation in spi_master.h"

void spi_init(void)
{
    // Configure and enable SPI peripheral
    SPCR = SPI_SPCR;
    SPSR = SPI_SPSR;
}

void spi_init2(u32_t bit_rate, u8_t spi_mode, bool_t data_order_lsb)
{
    u8_t  spcr;
    u32_t actual_bit_rate;
    u8_t  divisor;

    // Default values for SPCR
    spcr =  (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)
           |(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);

    // Set data order
    if(data_order_lsb == TRUE)
    {
        BIT_SET_HI(spcr, DORD);
    }

    // Set SPI Mode (0,1,2 or 3)
    spcr |= (spi_mode&3) << CPHA;

    // Select clock divisor that generates a bit rate closest to the desired bit rate (equal or less)
    actual_bit_rate = F_CPU;
    divisor = 1;
    do
    {
        actual_bit_rate >>= 1;
        divisor         <<= 1;
        // Stop if divisor reaches maximum
        if(divisor == 128)
        {
            break;
        }
    }
    while (actual_bit_rate > bit_rate);

    switch(divisor)
    {
    case 2 : spcr |= (0<<SPR1)|(0<<SPR0);
             BIT_SET_HI(SPSR, SPI2X);
             break;
    case 4 : spcr |= (0<<SPR1)|(0<<SPR0);
             BIT_SET_LO(SPSR, SPI2X);
             break;
    case 8 : spcr |= (0<<SPR1)|(1<<SPR0);
             BIT_SET_HI(SPSR, SPI2X);
             break;
    case 16 : spcr |= (0<<SPR1)|(1<<SPR0);
             BIT_SET_LO(SPSR, SPI2X);
             break;
    case 32 : spcr |= (1<<SPR1)|(0<<SPR0);
             BIT_SET_HI(SPSR, SPI2X);
             break;
    case 64 : spcr |= (1<<SPR1)|(0<<SPR0);
             BIT_SET_LO(SPSR, SPI2X);
             break;
    case 128 : spcr |= (1<<SPR1)|(1<<SPR0);
             BIT_SET_LO(SPSR, SPI2X);
             break;
    default:
             break;
    }

    SPCR = spcr;
}

void spi_tx_byte(u8_t tx_data)
{
    u8_t rx_data;

    // Buffer transmit data
    SPDR = tx_data;

    // Wait until data has been transfered
    LOOP_UNTIL_BIT_IS_HI(SPSR,SPIF);

    // Read received data
    rx_data = SPDR;
}

u8_t spi_rx_byte(void)
{
    u8_t rx_data;

    // Buffer dummy data
    SPDR = 0x00;

    // Wait until data has been transfered
    LOOP_UNTIL_BIT_IS_HI(SPSR,SPIF);

    // Read received data
    rx_data = SPDR;

    return rx_data;
}

u8_t spi_transfer_byte(u8_t tx_data)
{
    u8_t rx_data;

    // Buffer transmit data
    SPDR = tx_data;

    // Wait until data has been transfered
    LOOP_UNTIL_BIT_IS_HI(SPSR,SPIF);

    // Read received data
    rx_data = SPDR;

    return rx_data;
}

void spi_tx_data(const void *tx_data, size_t bytes_to_transmit)
{
    u8_t data;
    const u8_t *tx_data_byte = (u8_t *)tx_data;

    while(bytes_to_transmit)
    {
        // Buffer transmit data
        SPDR = *tx_data_byte++;
    
        // Wait until data has been transfered
        LOOP_UNTIL_BIT_IS_HI(SPSR,SPIF);
    
        // Read received data
        data = SPDR;

        // Next byte
        bytes_to_transmit--;
    }
}

void spi_rx_data(void *rx_data, size_t bytes_to_receive)
{
    u8_t data;
    u8_t *rx_data_byte = (u8_t *)rx_data;

    while(bytes_to_receive)
    {
        // Buffer dummy data
        SPDR = 0x00;
    
        // Wait until data has been transfered
        LOOP_UNTIL_BIT_IS_HI(SPSR,SPIF);
    
        // Read received data
        data = SPDR;

        // Buffer received data
        *rx_data_byte++ = data;

        // Next byte
        bytes_to_receive--;
    }
}

void spi_transfer_data(const void *tx_data, void *rx_data, size_t bytes_to_transfer)
{
    u8_t data;
    const u8_t *tx_data_byte = (u8_t *)tx_data;
    u8_t *rx_data_byte       = (u8_t *)rx_data;

    while(bytes_to_transfer)
    {
        // Buffer transmit data
        SPDR = *tx_data_byte++;
    
        // Wait until data has been transfered
        LOOP_UNTIL_BIT_IS_HI(SPSR,SPIF);
    
        // Read received data
        data = SPDR;

        // Buffer received data
        *rx_data_byte++ = data;

        // Next byte
        bytes_to_transfer--;
    }
}

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : Pieter.Conradie
 - First release
 
 2010-03-13 : Pieter.Conradie
 - Made SPCR and SPSR overridable with #defines
 - Added spi_init2(...)
 
 2010-03-15 : Alexandr Litjagin
 - Changed parameters of spi_tx_data(), spi_rx_data() and spi_transfer_data()
   from (u8_t *) to (void *) to ease the use of other types (e.g. structures)
   without having to resort to casting.
 
 2010-03-16 : Pieter.Conradie
 - Moved initialisation of SPI pins to board.c
 
 2010-03-17 : Pieter.Conradie
 - Added #warning regarding SS pin. See documentation in spi_master.h
   
*/
