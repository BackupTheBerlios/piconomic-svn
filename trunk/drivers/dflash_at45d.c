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

    Title:          Atmel AT45D Serial DataFlash Driver
    Author(s):      Pieter Conradie
    Creation Date:  2010-04-15
    Revision Info:  $Id: $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "dflash_at45d.h"
#include "spi1.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#if   defined(AT45DB011D) || defined(AT45DB021D)||defined(AT45DB081D)||defined(AT45DB161D)||defined(AT45DB321D)||defined(AT45DB642D)
#warning "The driver has not been tested with this device! Please email feedback."
#endif

/// \name Read commands
//@{
#define AT45D_CMD_CONTINUOUS_ARRAY_READ        0xE8
#define AT45D_CMD_MAIN_MEMORY_PAGE_READ        0xD2
#define AT45D_CMD_STATUS_REGISTER_READ         0xD7
//@}

/// \name Program and Erase commands
//@{
#define AT45D_CMD_BUFFER1_WRITE                0x84
#define AT45D_CMD_BUFFER2_WRITE                0x87
#define AT45D_CMD_BUF1_TO_MAIN_PAGE_PROGRAM    0x83
#define AT45D_CMD_BUF2_TO_MAIN_PAGE_PROGRAM    0x86
#define AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF1   0x82
#define AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF2   0x85
#define AT45D_CMD_PAGE_ERASE                   0x81
//@}

/// \name Additional commands
//@{
#define AT45D_CMD_MAIN_MEM_PAGE_TO_BUF1        0x53
#define AT45D_CMD_MAIN_MEM_PAGE_TO_BUF2        0x55
//@}

/* _____MACROS_______________________________________________________________ */
#ifndef AT45D_CS_LO
/// Macro to select Serial Flash chip (setting SFLASH CS pin low)
#define AT45D_CS_LO() board_flash_cs_lo()
#endif

#ifndef AT45D_CS_HI
/// Macro to deselect Serial Flash chip (setting SFLASH CS pin high)
#define AT45D_CS_HI() board_flash_cs_hi()
#endif

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static bool_t at45d_ready_flag;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
#if ((AT45D_PAGE_SIZE != 256) && (AT45D_PAGE_SIZE != 264))
#error "This driver does not work for any other page size yet. See comment!"
/*
 * Unfortunately this driver still needs work to cater for other page sizes. 
 *  
 * When the address is sent, the specified page value is shifted by a fixed amount, 
 * but actually the amount of shift depends on the page size (?) 
 */
#endif

static void at45d_send_address(u16_t page, u16_t start_byte_in_page)
{
#if VAL_IS_PWR_OF_TWO(AT45D_PAGE_SIZE)
    spi1_tx_byte((u8_t)(page>>8)&0xFF);
    spi1_tx_byte((u8_t)(page&0xFF));
    spi1_tx_byte((u8_t)(start_byte_in_page&0xFF));
#else
    spi1_tx_byte((page>>7)&0xFF);
    spi1_tx_byte(((page<<1)|(start_byte_in_page>>8))&0xFF);
    spi1_tx_byte(start_byte_in_page&0xFF);
#endif
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void at45d_init(void)
{
    // Set flag
    at45d_ready_flag = TRUE;
}

extern u16_t at45d_read(void       *buffer,
                       at45d_adr_t address,
                       u16_t       number_of_bytes)
{
    at45d_adr_t max_bytes_to_read;
    u16_t       page;
    u16_t       start_byte_in_page;

    // See if specified address is out of bounds
    if(address > AT45D_ADR_MAX)
    {
        return 0;
    }

    // See if "number of bytes to read" should be clipped
    max_bytes_to_read = AT45D_ADR_MAX - address + 1;
    if(number_of_bytes > max_bytes_to_read)
    {
        number_of_bytes = max_bytes_to_read;
    }

    // Wait until Flash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select serial Flash
    AT45D_CS_LO();

    // Send command
    spi1_tx_byte(AT45D_CMD_CONTINUOUS_ARRAY_READ);

    // Calculate page, offset and number of bytes remaining in page
#if VAL_IS_PWR_OF_TWO(AT45D_PAGE_SIZE)
    page               = address >> 8;
    start_byte_in_page = address & 0xff;
#else
    page               = address / AT45D_PAGE_SIZE;
    start_byte_in_page = address % AT45D_PAGE_SIZE;
#endif
    
    // Send address
    at45d_send_address(page, start_byte_in_page);

    // Send dont-care bits
    spi1_tx_byte(0x00);
    spi1_tx_byte(0x00);
    spi1_tx_byte(0x00);
    spi1_tx_byte(0x00);

    // Read data
    spi1_rx_data(buffer, number_of_bytes);

    // Deselect serial Flash
    AT45D_CS_HI();

    return number_of_bytes;
}

void at45d_read_page(void* buffer, u16_t page)
{
    // Wait until Flash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select serial Flash
    AT45D_CS_LO();

    // Send command
    spi1_tx_byte(AT45D_CMD_MAIN_MEMORY_PAGE_READ);

    // Send address
    at45d_send_address(page,0);

    // Send dont-care bits
    spi1_tx_byte(0x00);
    spi1_tx_byte(0x00);
    spi1_tx_byte(0x00);
    spi1_tx_byte(0x00);

    // Read data
    spi1_rx_data(buffer, AT45D_PAGE_SIZE);

    // Deselect serial Flash
    AT45D_CS_HI();
}

void at45d_read_page_offset(void *buffer,
                            u16_t page,
                            u16_t start_byte_in_page,
                            u16_t number_of_bytes)
{
    // Wait until Flash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select serial Flash
    AT45D_CS_LO();

    // Send command
    spi1_tx_byte(AT45D_CMD_CONTINUOUS_ARRAY_READ);

    // Send address
    at45d_send_address(page, start_byte_in_page);

    // Send dont-care bits
    spi1_tx_byte(0x00);
    spi1_tx_byte(0x00);
    spi1_tx_byte(0x00);
    spi1_tx_byte(0x00);

    // Read data
    spi1_rx_data(buffer, number_of_bytes);    

    // Deselect serial Flash
    AT45D_CS_HI();
}

void at45d_write_page(const void* buffer, u16_t page)
{
    // Wait until Flash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select serial Flash
    AT45D_CS_LO();

    // Send command
    spi1_tx_byte(AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF1);

    // Send address
    at45d_send_address(page, 0);

    // Send data to be written
    spi1_tx_data(buffer, AT45D_PAGE_SIZE);    

    // Deselect serial Flash
    AT45D_CS_HI();

    // Set flag to busy
    at45d_ready_flag = FALSE;
}

void at45d_write_page_offset(const void* buffer,
                             u16_t       page,
                             u16_t       start_byte_in_page,
                             u16_t       number_of_bytes)
{
    // Wait until Flash is not busy
    while(!at45d_ready())
    {
        ;
    }   

    // Select serial Flash
    AT45D_CS_LO();

    // Send command
    spi1_tx_byte(AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF1);

    // Send address
    at45d_send_address(page, start_byte_in_page);

    // Send data to be written
    spi1_tx_data(buffer, number_of_bytes);    

    // Deselect serial Flash
    AT45D_CS_HI();

    // Set flag to busy
    at45d_ready_flag = FALSE;
}

void at45d_erase_page(u16_t page)
{
    // Wait until Flash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select serial Flash
    AT45D_CS_LO();

    // Send command
    spi1_tx_byte(AT45D_CMD_PAGE_ERASE);

    // Send address
    at45d_send_address(page, 0);

    // Deselect serial Flash
    AT45D_CS_HI();

    // Set flag to busy
    at45d_ready_flag = FALSE;
}

bool_t at45d_ready(void)
{
    u8_t data;

    // If flag has already been set, then take short-cut
    if(at45d_ready_flag)
    {
        return TRUE;
    }

    // Get Flash status
    data = at45d_get_status();

    // See if Flash is ready
    if(BIT_IS_HI(data, AT45D_STATUS_READY))
    {
        // Set flag
        at45d_ready_flag = TRUE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

u8_t at45d_get_status(void)
{
    u8_t data;

    // Select serial Flash
    AT45D_CS_LO();

    // Send command
    spi1_tx_byte(AT45D_CMD_STATUS_REGISTER_READ);

    // Read status
    data = spi1_rx_byte();

    // Deselect serial Flash
    AT45D_CS_HI();

    return data;
}

bool_t at45d_page_size_is_pwr_of_two(void)
{
    u8_t data = at45d_get_status();

    if(BIT_IS_HI(data, AT45D_STATUS_PAGE_SIZE))
    {
        // Page size is a power of two
        return TRUE;
    }
    else
    {
        // Page size is not a power of two
        return FALSE;
    }
}

extern bool_t at45d_set_page_size_to_pwr_of_two(void)
{
    u8_t data = at45d_get_status();

    if(BIT_IS_HI(data,AT45D_STATUS_PAGE_SIZE))
    {
        // Page size is already a power of two
        return FALSE;
    }

    // Wait until Flash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select serial Flash
    AT45D_CS_LO();

    // Send command
    spi1_tx_byte(0x3D);
    spi1_tx_byte(0x2A);
    spi1_tx_byte(0x80);
    spi1_tx_byte(0xA6);    

    // Deselect serial Flash
    AT45D_CS_HI();

    // Wait until Flash is not busy
    while(!at45d_ready())
    {
        ;
    }

    return TRUE;
}

/**
 *  @}
 */

/* _____LOG__________________________________________________________________ */
/*

 2010-04-15 : Pieter.Conradie
 - First release
   
*/
