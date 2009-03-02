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

    Title:          Real Time Clock using 32.768 kHz crystal and Timer 0
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: rtc_tmr0_32khz.c 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "rtc_tmr0_32khz.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#define RTC_PERIOD_MS 1000

/* _____LOCAL VARIABLES______________________________________________________ */
/// Structure to keep track of time & date
static rtc_time_t rtc_time;

/// Table for the number of days in each month (non leap year)
static prog_uint8_t rtc_month_day_table[] = {
                                               0,   // Invalid month
                                               31,  // January
                                               28,  // February
                                               31,  // March
                                               30,  // April
                                               31,  // May
                                               30,  // June
                                               31,  // July
                                               31,  // August
                                               30,  // September
                                               31,  // October
                                               30,  // November
                                               31   // December
                                              };

/* _____PRIVATE FUNCTIONS____________________________________________________ */
/// Interrupt handler on TMR0 compare match (TCNT0 is cleared automatically)
ISR(TIMER0_COMP_vect)
{
    u8_t days_in_month;

    // Increment seconds
    if(++rtc_time.sec < 60) return;
    rtc_time.sec = 0;

    // Increment minutes
    if(++rtc_time.min < 60) return;
    rtc_time.min = 0;

    // Increment hours
    if(++rtc_time.hour < 24) return;
    rtc_time.hour = 0;

    // Determine number of days in month using table in program memory
    days_in_month = pgm_read_byte_near(&rtc_month_day_table[rtc_time.month]);

    if(rtc_time.month == 2)
    {
        // Adjust number of days in February acording to Gregorian calender rules
        if(((rtc_time.year)%4) == 0)
        {
            // Every 4 years is a leap year...
            days_in_month = 29;
            if(((rtc_time.year)%100) == 0)
            {
                // Every 100 years is not a leap year...
                days_in_month = 28;
                if(((rtc_time.year)%400) == 0)
                {
                    // Every 400 years is a leap year after all
                    days_in_month = 29;
                }
            }
        }
    }

    // Increment days
    if(++rtc_time.day <= days_in_month) return;
    rtc_time.day = 1;

    // Increment months
    if(++rtc_time.month <= 12) return;
    rtc_time.month = 1;

    // Increment years
    rtc_time.year++;
}

/* _____FUNCTIONS_____________________________________________________ */
void rtc_init(void)
{
    // Reset time
    rtc_time.sec   = 0;
    rtc_time.min   = 0;
    rtc_time.hour  = 12;
    rtc_time.day   = 1;
    rtc_time.month = 1;
    rtc_time.year  = 2000;

    // Select asynchronous timer 0 operation to use external 32.768 kHz crystal
    BIT_SET_HI(ASSR, AS0);

    // Start timer 0 with clock prescaler CLK/1024 and CTC Mode ("Clear Timer on Compare")
    // Resolution is 32.25 ms
    TCCR0 = (0<<FOC0)|(0<<WGM00)|(0<<COM01)|(0<<COM00)|(1<<WGM01)|(1<<CS02)|(1<<CS01)|(1<<CS00);

    // Reset time
    TCNT0 = 0;

    // Calculate and set period
    OCR0 = (u16_t)(((F_RTC/1024)*RTC_PERIOD_MS)/1000) - 1;

    // Wait for "update busy" flags to clear
    while(ASSR&((1<<TCN0UB)|(1<<OCR0UB)|(1<<TCR0UB)))
    {
        ;
    }

    // Enable interrupt on compare match
    BIT_SET_HI(TIMSK, OCIE0);
}

void rtc_get_time(rtc_time_t* time)
{
    // Disable interrupt
    BIT_SET_LO(TIMSK, OCIE0);

    // Copy time into structure
    memcpy(time, &rtc_time, sizeof(*time));

    // Enable interrupt
    BIT_SET_HI(TIMSK, OCIE0);
}


void rtc_set_time(const rtc_time_t* time)
{
    // Disable interrupt
    BIT_SET_LO(TIMSK, OCIE0);

    // Copy time into structure
    memcpy(&rtc_time, time, sizeof(rtc_time));

    // Enable interrupt
    BIT_SET_HI(TIMSK, OCIE0);
}

/**
 *  @}
 */

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : PJC
 - First release
   
*/
