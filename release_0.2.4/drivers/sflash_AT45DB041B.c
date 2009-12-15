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
   
    Title:          Atmel AT45DB041B 4-megabit DataFlash Driver
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "sflash_AT45DB041B.h"
#include "spi_master.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// \name Read commands
//@{
#define AT45DB041B_CMD_CONTINUOUS_ARRAY_READ        0xE8
#define AT45DB041B_CMD_MAIN_MEMORY_PAGE_READ        0xD2
#define AT45DB041B_CMD_STATUS_REGISTER_READ         0xD7
//@}

/// \name Program and Erase commands
//@{
#define AT45DB041B_CMD_BUFFER1_WRITE                0x84
#define AT45DB041B_CMD_BUFFER2_WRITE                0x87
#define AT45DB041B_CMD_BUF1_TO_MAIN_PAGE_PROGRAM    0x83
#define AT45DB041B_CMD_BUF2_TO_MAIN_PAGE_PROGRAM    0x86
#define AT45DB041B_CMD_MAIN_MEM_PROG_THROUGH_BUF1   0x82
#define AT45DB041B_CMD_MAIN_MEM_PROG_THROUGH_BUF2   0x85
//@}

/// \name Additional commands
//@{
#define AT45DB041B_CMD_MAIN_MEM_PAGE_TO_BUF1        0x53
#define AT45DB041B_CMD_MAIN_MEM_PAGE_TO_BUF2        0x55
//@}

/* _____MACROS_______________________________________________________________ */
/// Macro to insert an AVR assembly instruction delay of one clock cycle
#define AT45DB041B_NOP() \
           __asm__ __volatile__("nop    \n\t"::);

/// Macro to select Serial Flash chip (setting SFLASH CS pin low)
#define AT45DB041B_CS_LO() \
           BIT_SET_LO(PORT_SFLASH_CS_O,BIT_SFLASH_CS_O); \
           AT45DB041B_NOP(); /* Delay for minimum CS setup time */ \
           AT45DB041B_NOP();

/// Macro to deselect Serial Flash chip (setting SFLASH CS pin high)
#define AT45DB041B_CS_HI() \
           AT45DB041B_NOP()  /* Delay for minimum CS hold time */ \
           AT45DB041B_NOP()\
           BIT_SET_HI(PORT_SFLASH_CS_O,BIT_SFLASH_CS_O); \
           AT45DB041B_NOP(); /* Delay for minimum CS high time */ \
           AT45DB041B_NOP();

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static bool_t at45db041b_ready_flag;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void at45db041b_init(void)
{
    // Set flag
    at45db041b_ready_flag = TRUE;
}

void at45db041b_read_page(u16_t page, u8_t* buffer)
{
    u8_t  data;
    u16_t counter;

    // Wait until Flash is not busy
    while(!at45db041b_ready())
    {
        ;
    }

    // Select serial Flash
    AT45DB041B_CS_LO();

    // Send command
    spi_tx_byte(AT45DB041B_CMD_MAIN_MEMORY_PAGE_READ);

    // Send address
    spi_tx_byte((u8_t)(page>>7)&0xFF);
    spi_tx_byte((u8_t)(page<<1)&0xFF);
    spi_tx_byte(0x00);

    // Send dont-care bits
    spi_tx_byte(0x00);
    spi_tx_byte(0x00);
    spi_tx_byte(0x00);
    spi_tx_byte(0x00);

    // Read data
    spi_rx_data(buffer, AT45DB041B_PAGE_SIZE);    

    // Deselect serial Flash
    AT45DB041B_CS_HI();
}

void at45db041b_read_data(u16_t page,
                          u16_t start_byte_in_page,
                          u8_t *buffer,
                          u16_t number_of_bytes)
{
    u8_t data;

    // Wait until Flash is not busy
    while(!at45db041b_ready())
    {
        ;
    }

    // Select serial Flash
    AT45DB041B_CS_LO();

    // Send command
    spi_tx_byte(AT45DB041B_CMD_CONTINUOUS_ARRAY_READ);

    // Send address
    spi_tx_byte((page>>7)&0xFF);
    spi_tx_byte(((page<<1)|(start_byte_in_page>>8))&0xFF);
    spi_tx_byte(start_byte_in_page&0xFF);

    // Send dont-care bits
    spi_tx_byte(0x00);
    spi_tx_byte(0x00);
    spi_tx_byte(0x00);
    spi_tx_byte(0x00);

    // Read data
    spi_rx_data(buffer, number_of_bytes);    

    // Deselect serial Flash
    AT45DB041B_CS_HI();
}

void at45db041b_write_page(u16_t page, const u8_t* buffer)
{
    u8_t  data;
    u16_t counter;

    // Wait until Flash is not busy
    while(!at45db041b_ready())
    {
        ;
    }

    // Select serial Flash
    AT45DB041B_CS_LO();

    // Send command
    spi_tx_byte(AT45DB041B_CMD_MAIN_MEM_PROG_THROUGH_BUF1);

    // Send address
    spi_tx_byte((u8_t)(page>>7)&0xFF);
    spi_tx_byte((u8_t)(page<<1)&0xFF);
    spi_tx_byte(0x00);   

    // Send data to be written
    spi_tx_data(buffer, AT45DB041B_PAGE_SIZE);    

    // Deselect serial Flash
    AT45DB041B_CS_HI();

    // Set flag to busy
    at45db041b_ready_flag = FALSE;
}

void at45db041b_write_data(u16_t       page,
                           u16_t       start_byte_in_page,
                           const u8_t* buffer,
                           u16_t       number_of_bytes)
{
    u8_t data;

    // Wait until Flash is not busy
    while(!at45db041b_ready())
    {
        ;
    }   

    // Select serial Flash
    AT45DB041B_CS_LO();

    // Send command
    spi_tx_byte(AT45DB041B_CMD_MAIN_MEM_PAGE_TO_BUF1);

    // Send address
    spi_tx_byte((page>>7)&0xFF);
    spi_tx_byte((page<<1)&0xFF);
    spi_tx_byte(0x00);

    // Deselect serial Flash
    AT45DB041B_CS_HI();

    // Set flag to busy
    at45db041b_ready_flag = FALSE;

    // Wait for transfer to finish
    while(!at45db041b_ready())
    {
        ;
    }

    // Select serial Flash
    AT45DB041B_CS_LO();

    // Send command
    spi_tx_byte(AT45DB041B_CMD_MAIN_MEM_PROG_THROUGH_BUF1);

    // Send address
    spi_tx_byte((page>>7)&0xFF);
    spi_tx_byte(((page<<1)|(start_byte_in_page>>8))&0xFF);
    spi_tx_byte(start_byte_in_page&0xFF);

    // Send data to be written
    spi_tx_data(buffer, number_of_bytes);    

    // Deselect serial Flash
    AT45DB041B_CS_HI();

    // Set flag to busy
    at45db041b_ready_flag = FALSE;
}

bool_t at45db041b_ready(void)
{
    u8_t data;

    // If flag has already been set, then take short-cut
    if(at45db041b_ready_flag)
    {
        return TRUE;
    }

    // Get Flash status
    data = at45db041b_get_status();

    // See if Flash is ready
    if(BIT_IS_HI(data, AT45DB041B_STATUS_READY))
    {
        // Set flag
        at45db041b_ready_flag = TRUE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

u8_t at45db041b_get_status(void)
{
    u8_t data;

    // Select serial Flash
    AT45DB041B_CS_LO();

    // Send command
    spi_tx_byte(AT45DB041B_CMD_STATUS_REGISTER_READ);

    // Read status
    data = spi_rx_byte();

    // Deselect serial Flash
    AT45DB041B_CS_HI();

    return data;
} // Doxygen end of example

/**
 *  @}
 */

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : PJC
 - First release
   
*/
