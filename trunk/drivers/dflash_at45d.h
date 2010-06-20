#ifndef __DFLASH_AT45D_H__
#define __DFLASH_AT45D_H__
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
/** 
 *  @ingroup DRIVERS
 *  @defgroup AT45D dflash_at45d.h : Atmel AT45D DataFlash Driver
 *
 *  Driver that communicates with an Atmel AT45D DataFlash using SPI.
 *
 *  Files: dflash_at45d.h & dflash_at45d.c
 *
 *  @par Reference:
 *  - <a href="http://www.atmel.com/dyn/resources/prod_documents/doc3595.pdf">Atmel 4M bit 2.5-Volt or 2.7-Volt DataFlash AT45DB041D datasheet</a>
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* __DEFINITIONS ____________________________________________________________ */
// Definitions for the benefit of Doxygen
#ifdef __DOXYGEN__
#define AT45D_POWER_OF_TWO_PAGE_SIZE 0
#define AT45DB041D
#define AT45D_PAGES             2048UL
#define AT45D_PAGE_SIZE         264
#endif

/*
 *  "Power of 2" binary page size Configuration Register is a user-programmable
 *  nonvolatile register that allows the page size of the main memory to be
 *  configured for binary page size or the DataFlash standard page size.
 *  
 *  See at45d_set_page_size_to_power_of_two().
 */
#ifndef AT45D_POWER_OF_TWO_PAGE_SIZE
#error "AT45D_POWER_OF_TWO_PAGE_SIZE must be defined to 0 or 1 to select page size"
#endif

// AT45DB011D: 1M bit, 2.7-Volt Minimum Serial-Interface Flash with One 264-Byte SRAM Buffer
#if   defined(AT45DB011D)
#define AT45D_PAGES             512UL
#if AT45D_POWER_OF_TWO_PAGE_SIZE
#define AT45D_PAGE_SIZE         256
#else
#define AT45D_PAGE_SIZE         264
#endif
// AT45DB021D: 2M bit, 2.7-Volt Minimum Serial-Interface Flash with One 264-Byte SRAM Buffer
#elif defined(AT45DB021D)
#define AT45D_PAGES             1024UL
#if AT45D_POWER_OF_TWO_PAGE_SIZE
#define AT45D_PAGE_SIZE         256
#else
#define AT45D_PAGE_SIZE         264
#endif
// AT45DB041D: 4M bit 2.5-Volt or 2.7-Volt DataFlash
#elif defined(AT45DB041D)
#define AT45D_PAGES             2048UL
#if AT45D_POWER_OF_TWO_PAGE_SIZE
#define AT45D_PAGE_SIZE         256
#else
#define AT45D_PAGE_SIZE         264
#endif
#define AT45D_DENSITY           0x1C
// AT45DB081D: 8M bit, 2.5 or 2.7-Volt Only Serial-Interface Flash
#elif defined(AT45DB081D)
#define AT45D_PAGES             4096UL
#if AT45D_POWER_OF_TWO_PAGE_SIZE
#define AT45D_PAGE_SIZE         256
#else
#define AT45D_PAGE_SIZE         264
#endif
// AT45DB161D: 16M bit, 2.7-Volt Only Serial-Interface Flash with two SRAM Data Buffers
#elif defined(AT45DB161D)
#define AT45D_PAGES             4096UL
#if AT45D_POWER_OF_TWO_PAGE_SIZE
#define AT45D_PAGE_SIZE         512
#else
#define AT45D_PAGE_SIZE         528
#endif
// AT45DB321D:  32M bit, 2.7-Volt Only Serial Interface Flash
#elif defined(AT45DB321D)
#define AT45D_PAGES             8192UL
#if AT45D_POWER_OF_TWO_PAGE_SIZE
#define AT45D_PAGE_SIZE         512
#else
#define AT45D_PAGE_SIZE         528
#endif
// AT45DB642D: 64M bit, 2.7-Volt Dual-Interface Flash with two 1056-Byte SRAM
#elif defined(AT45DB642D)
#define AT45D_PAGES             8192UL
#if AT45D_POWER_OF_TWO_PAGE_SIZE
#define AT45D_PAGE_SIZE         1024
#else
#define AT45D_PAGE_SIZE         1056
#endif

#else
#error "No compatible AT45D device selected"
#endif


/// Status register - Ready flag
#define AT45D_STATUS_READY      7
/// Status register - Compare flag
#define AT45D_STATUS_COMP       6
/// Status register - Density mask
#define AT45D_DENSITY_MASK      0x3C
/// Status register - Protect flag
#define AT45D_STATUS_PROTECT    1
/// Status register - Page size flag
#define AT45D_STATUS_PAGE_SIZE  0

/// Flash size (in bytes)
#define AT45D_FLASH_SIZE_BYTES   (AT45D_PAGES*AT45D_PAGE_SIZE)

/// Maximum adress
#define AT45D_ADR_MAX            (AT45D_FLASH_SIZE_BYTES-1)

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Address size can be optimized, depending on maximum size of FLASH
#if (AT45D_ADR_MAX <= 0xffff)
typedef u16_t at45d_adr_t;
#else
typedef u32_t at45d_adr_t;
#endif

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise driver flags.
 */
extern void at45d_init(void);

/**
 *  Read data from DataFlash.
 *  
 *  This function reads data from DataFlash and stores it in the 
 *  specified buffer.
 * 
 *  @param[out] buffer          Buffer to store read data
 *  @param[in]  address         0 to AT45D_ADR_MAX
 *  @param[in]  number_of_bytes Number of bytes to read
 *  
 *  @return u16_t               Number of bytes actually read
 */
extern u16_t at45d_read(void        *buffer,
                        at45d_adr_t address,
                        u16_t       number_of_bytes);

/**
 *  Read a page from DataFlash.
 *  
 *  This function reads a page of data from DataFlash and stores it in the 
 *  specified buffer. The buffer must be at least #AT45D_PAGE_SIZE bytes
 *  in size to accomodate a full page.
 * 
 *  The DataFlash has #AT45D_PAGES pages.
 * 
 *  @param[out] buffer          Buffer to store read data
 *  @param[in]  page            0 to (#AT45D_PAGES-1)
 *  
 */
extern void at45d_read_page(void  *buffer,
                            u16_t page);

/**
 *  Partial read of data in a page of DataFlash.
 * 
 *  This function reads part of a page of data from DataFlash and stores it 
 *  in the specified buffer. The buffer must be at least @b number_of_bytes
 *  in size to the read data.
 * 
 *  @note Only read up to the end of the specified page. If the page boundary is
 *         exceeded, the index will wrap to the start of the page, i.e. only 
 *        the content of the specified page will be read.
 * 
 *  @param[out] buffer              Buffer to store read data
 *  @param[in]  page                0 to (#AT45D_PAGES-1)
 *  @param[in]  start_byte_in_page  Index of first byte to read (0 to
 *                                  #AT45D_PAGE_SIZE - 1)
 *  @param[in]  number_of_bytes     Number of bytes to read
 * 
 */
extern void at45d_read_page_offset(void  *buffer,
                                   u16_t page,
                                   u16_t start_byte_in_page,
                                   u16_t number_of_bytes);

/**
 *  Write a page from DataFlash.
 *  
 *  This function writes a page of data to DataFlash using the specified 
 *  buffer as the source. The buffer must contain at least #AT45D_PAGE_SIZE bytes
 *  of data.
 * 
 *  The DataFlash has #AT45D_PAGES pages.
 * 
 *  @param[in] buffer  Buffer containing data to be written
 *  @param[in]  page   0 to (#AT45D_PAGES-1)
 *  
 */
extern void at45d_write_page(const void *buffer,
                             u16_t      page);

/**
 *  Partial write of data in a page of DataFlash.
 * 
 *  This function writes a part of a page of data to DataFlash using the 
 *  specified buffer as source. The buffer must contain at least @b number_of_bytes
 *  of data.
 * 
 *  @note Only write up to the end of the specified page. If the page boundary
 *        is exceeded, the index will wrap to the start of the page, i.e. only 
 *        the content of the specified page will be written.
 * 
 *  @param[in]  buffer              Buffer containing data to be written
 *  @param[in]  page                0 to (#AT45D_PAGES-1)
 *  @param[in]  start_byte_in_page  Index of first byte to write (0 to
 *                                  #AT45D_PAGE_SIZE - 1)
 *  @param[in]  number_of_bytes     Number of bytes to write
 * 
 */
extern void at45d_write_page_offset(const void *buffer, 
                                    u16_t      page,   
                                    u16_t      start_byte_in_page,                                    
                                    u16_t      number_of_bytes);

/**
 *  Erase a page of DataFlash.
 *  
 *  This function erases a page of DataFlash. The DataFlash has
 *  #AT45D_PAGES pages.
 * 
 *  @param[in]  page   0 to (#AT45D_PAGES-1)
 */
extern void at45d_erase_page(u16_t page);

/**
 *  Check if DataFlash is ready for the next read or write access.
 * 
 *  When data is written to DataFlash, the microcontroller must wait
 *  until the operation is finished, before the next one is attempted. 
 *  This function allows the microcontroller to do something else while 
 *  waiting for the operation to finish.
 * 
 * @retval TRUE     DataFlash is ready for next read or write access.
 * @retval FALSE    DataFlash is busy writing.
 */
extern bool_t at45d_ready(void);

/**
 *  Read the status register of the DataFlash.
 * 
 *  The status register contains flags (e.g. #AT45D_STATUS_READY)
 *  and the density value (e.g. #AT45D_DENSITY). This function can be used
 *  to check that a valid and working DataFlash device is connected.
 * 
 *  @return u8_t Status register value
 */
extern u8_t at45d_get_status(void);

/**
 *  Check if page size of the DataFlash is a power of two.
 * 
 *  The status register contains a flag #AT45D_STATUS_PAGE_SIZE that
 *  reports the page size: 1 = power of two, 0 = not a power of two.
 * 
 *  @return bool_t TRUE  Page size is a power of two
 *                 FALSE Page size is not a power of two
 */
extern bool_t at45d_page_size_is_pwr_of_two(void);

/**
 *  Sets the page size of the DataFlash to a power of two.
 * 
 *  A one-time programmable configuration register is written to configure
 *  the device for “power of 2” page size. Once it has been written, it can not
 *  be reconfigured again.
 *  
 *  The device must be power cycled for this configuration bit to come into
 *  effect.
 *  
 *  @return bool_t TRUE     page size was set to a power of two
 *                 FALSE    page size was already set to a power of two
 */
extern bool_t at45d_set_page_size_to_pwr_of_two(void);

/* _____MACROS_______________________________________________________________ */
/**
 *  @}
 */
#endif
