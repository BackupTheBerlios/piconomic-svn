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

    Title:          Driver for an ST M41T00 Real Time Clock
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <compat/twi.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "rtc_m41t00.h"
#include "twi_master.h"
#include "dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// TWI Address
#define M41T00_ADR 0xD0

/// M41T00 Data
typedef struct
{
    u8_t sec;
    u8_t min;
    u8_t century_hours;
    u8_t day_of_week;
    u8_t day_of_month;
    u8_t month;
    u8_t year;
} m41t00_bcd_time_t;

typedef struct
{
    u8_t                adr;
    m41t00_bcd_time_t   bcd_time;
    u8_t                control;
} m41t00_data_t;

/// Stop bit
#define M41T00_ST_ADR   0x00
#define M41T00_ST_BIT   (1<<7)

/// Century Enable Bit
#define M41T00_CEB_ADR  0x02
#define M41T00_CEB_BIT  (1<<7)

/// Century Bit
#define M41T00_CB_ADR   0x02
#define M41T00_CB_BIT   (1<<6)

// Control register map

/// Output level bit
#define M41T00_OUT_BIT  (1<<7)

/// Frequency test bit
#define M41T00_FT_BIT   (1<<6)

/// Sign bit
#define M41T00_S_BIT    (1<<5)

/// Clock calibration
#define M41T00_CAL_MASK 0x1F

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static m41t00_data_t m41t00_data;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static u8_t m41t00_bcd_to_byte(u8_t bcd_val)
{
    return (bcd_val&0xf) + ((bcd_val&0xf0) >> 4)*10;
}

static u8_t m41t00_byte_to_bcd(u8_t byte_val)
{
    return (byte_val%10) + ((byte_val/10) << 4);
}

static bool_t m41t00_read_data(void)
{
    // Set address to start reading from
    m41t00_data.adr = 0;

    // Send address to read
    twi_start_tx(M41T00_ADR, &m41t00_data.adr, sizeof(m41t00_data.adr));
    
    // Wait until TWI transfer is complete
    while(twi_busy())
    {
        ;
    }

    // See if transfer was successful
    if(!twi_success())
    {
        DBG_ERR("TWI Write transfer not successful. TWI STATUS = %d\n",twi_get_status());
        return FALSE;
    }

    // Start read transfer
    twi_start_rx(M41T00_ADR,
                 (u8_t*)&m41t00_data.bcd_time, 
                 sizeof(m41t00_data.bcd_time)+sizeof(m41t00_data.control));
    
    // Wait until TWI transfer is complete    
    while(twi_busy())
    {
        ;
    }

    // See if transfer was successful
    if(!twi_success())
    {
        DBG_ERR("TWI Read transfer not successful. TWI STATUS = %d\n",twi_get_status());
        return FALSE;
    }    

#if TWI_REPEATED_START_SUPPORT
    // Send explicit TWI stop
    twi_send_stop();
#endif

    return TRUE;
}

static bool_t m41t00_write_data(void)
{
    // Set address to start writing to
    m41t00_data.adr = 0;

    // Write data
    twi_start_tx(M41T00_ADR, (u8_t*)&m41t00_data, sizeof(m41t00_data));
    
    // Wait until TWI transfer is complete
    while(twi_busy())
    {
        ;
    }

    // See if transfer was successful
    if(!twi_success())
    {
        DBG_ERR("TWI Write transfer not successful. TWI STATUS = %d\n",twi_get_status());
        return FALSE;
    }

#if TWI_REPEATED_START_SUPPORT
    // Send explicit TWI stop
    twi_send_stop();
#endif

    return TRUE;
} // Doxygen end of example

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool_t m41t00_get_time(m41t00_time_t *rtc_time)
{    
    u8_t i;

    // Populate with default values
    rtc_time->sec          = 0;
    rtc_time->min          = 0;
    rtc_time->hour         = 0;
    rtc_time->day_of_week  = 1;
    rtc_time->day_of_month = 1;
    rtc_time->month        = 1;
    rtc_time->year         = 7;

    // Read RTC data over TWI interface
    if(!m41t00_read_data())
    {
        return FALSE;
    }

    // See if timer has been stopped
    if(((u8_t*)(&m41t00_data.bcd_time))[M41T00_ST_ADR]  & M41T00_ST_BIT)
    {
        DBG_PROG("RTC has been stopped!\n");
        return FALSE;
    }

    // Mask off century bits
    ((u8_t*)(&m41t00_data.bcd_time))[M41T00_CEB_ADR] &= ~M41T00_CEB_BIT;
    ((u8_t*)(&m41t00_data.bcd_time))[M41T00_CB_ADR]  &= ~M41T00_CB_BIT;

    // Convert to binary format
    for(i=0;i<sizeof(*rtc_time);i++)
    {
        ((u8_t*)rtc_time)[i] = m41t00_bcd_to_byte(((u8_t*)(&m41t00_data.bcd_time))[i]);
    }

    // Sanity check
    if((rtc_time->month        < 1   ) || (rtc_time->month      > 12  )) return FALSE;
    if((rtc_time->day_of_month < 1   ) || (rtc_time->day_of_month > 31  )) return FALSE;
    if(rtc_time->hour          > 23  ) return FALSE;
    if(rtc_time->min           > 59  ) return FALSE;
    if(rtc_time->sec           > 59  ) return FALSE;

    return TRUE;
}

bool_t m41t00_set_time(const m41t00_time_t* rtc_time)
{    
    int i;

    // Convert to BCD format
    for(i=0;i<sizeof(m41t00_data.bcd_time);i++)
    {
        ((u8_t*)(&m41t00_data.bcd_time))[i] = m41t00_byte_to_bcd(((u8_t*)rtc_time)[i]);
    }

    // Set control bits
    ((u8_t*)(&m41t00_data.bcd_time))[M41T00_ST_ADR]  &= ~M41T00_ST_BIT;
    ((u8_t*)(&m41t00_data.bcd_time))[M41T00_CEB_ADR] &= ~M41T00_CEB_BIT;
    ((u8_t*)(&m41t00_data.bcd_time))[M41T00_CB_ADR]  &= ~M41T00_CB_BIT;    

    m41t00_data.control = 0x00;

    return m41t00_write_data();
}

/**
 *  @}
 */

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : PJC
 - First release
   
*/
