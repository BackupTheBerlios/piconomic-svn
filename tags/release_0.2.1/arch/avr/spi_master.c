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
    Revision Info:  $Id: spi_master.c 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "spi_master.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void spi_init(void)
{
    // Enable SPI pins
    BIT_SET_HI(PORT_SPI,   BIT_SPI_SS);
    BIT_SET_LO(DDR_SPI,    BIT_SPI_SS);

    BIT_SET_HI(PORT_SPI,   BIT_SPI_SCK);
    BIT_SET_HI(DDR_SPI,    BIT_SPI_SCK);

    BIT_SET_HI(PORT_SPI,   BIT_SPI_MOSI);
    BIT_SET_HI(DDR_SPI,    BIT_SPI_MOSI);

    BIT_SET_LO(PORT_SPI,   BIT_SPI_MISO);
    BIT_SET_LO(DDR_SPI,    BIT_SPI_MISO);

    // Configure and enable SPI peripheral
    SPCR =  (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)
           |(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
    SPSR =  (0<<SPI2X);
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

void spi_tx_data(u8_t *tx_data, size_t bytes_to_transmit)
{
    u8_t data;

    while(bytes_to_transmit)
    {
        // Buffer transmit data
        SPDR = *tx_data++;
    
        // Wait until data has been transfered
        LOOP_UNTIL_BIT_IS_HI(SPSR,SPIF);
    
        // Read received data
        data = SPDR;

        // Next byte
        bytes_to_transmit--;
    }
}

void spi_rx_data(u8_t *rx_data, size_t bytes_to_receive)
{
    u8_t data;

    while(bytes_to_receive)
    {
        // Buffer dummy data
        SPDR = 0x00;
    
        // Wait until data has been transfered
        LOOP_UNTIL_BIT_IS_HI(SPSR,SPIF);
    
        // Read received data
        data = SPDR;

        // Buffer received data
        *rx_data++ = data;

        // Next byte
        bytes_to_receive--;
    }
}

void spi_transfer_data(u8_t *tx_data, u8_t *rx_data, size_t bytes_to_transfer)
{
    u8_t data;

    while(bytes_to_transfer)
    {
        // Buffer transmit data
        SPDR = *tx_data++;
    
        // Wait until data has been transfered
        LOOP_UNTIL_BIT_IS_HI(SPSR,SPIF);
    
        // Read received data
        data = SPDR;

        // Buffer received data
        *rx_data++ = data;

        // Next byte
        bytes_to_transfer--;
    }
}

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : PJC
 - First release
   
*/
