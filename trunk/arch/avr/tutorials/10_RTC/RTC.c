/* ==========================================================================

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

========================================================================== */

/**
 * @ingroup AVR_TUTORIALS
 * @defgroup AVR_TUT10_RTC /10_RTC
 *   
 * How to use a 32.768 kHz crystal and the asynchronous timer 0 as a Real Time Clock (RTC).
 * 
 * Files: RTC.c
 * 
 * The LED output is toggled once a second, and time updated (but not used in 
 * this tutorial). There is a common pitfall when flags or variables are updated
 * in an interrupt and the main program flow waits for these flags or variables.
 * 
 * The C keyword @b volatile is used to indicate to the compiler that variables
 * with this prefix are updated asynchronously, i.e. the variable was updated in 
 * an interrupt, but the main program flow is unaware when/if it happened. 
 * The compiler will generate assembler code that fetches the value of the 
 * variable each time it is referenced, in stead of optimizing the code by 
 * fetching once only. Here is example pseudo code that works correctly:
 * 
 * @code
 * volatile bool_t flag;
 * ISR(TIMER0_COMP_vect)
 *  {
 *     // Set flag
 *     flag = TRUE;
 *  }
 *  
 *  void function(void)
 *  {
 *     // Clear flag
 *     flag = FALSE;
 *  
 *     // Wait for timer interrupt to occur
 *     while(flag == FALSE) {;}
 *  
 *     // Do something after timer interrupt
 *  }
 * @endcode
 * 
 * @note
 * The RTC functionality has been encapsulated in @ref RTC_TMR0_32KHZ.
 * 
 * @{
 * 
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Output Port pin LED_O
#define PORT_LED_O      PORTB
#define DDR_LED_O       DDRB
#define BIT_LED_O       6

// External crystal frequency
#define F_RTC           32768

// Define delay
#define PERIOD_MS       1000

// Global time
volatile uint8_t tmr_sec;
volatile uint8_t tmr_min;
volatile uint8_t tmr_hour;
volatile uint8_t tmr_days;

// Interrupt handler on compare match (TCNT0 is cleared automatically)
ISR(TIMER0_COMP_vect)
{
    // Increment time
    if(++tmr_sec == 60)
    {
        tmr_sec = 0;
        if(++tmr_min == 60)
        {
            tmr_min = 0;
            if(++tmr_hour == 24)
            {
                tmr_hour = 0;
                tmr_days++;
            }
        }
    }

    // Toggle LED
    if(PORT_LED_O&(1<<BIT_LED_O))
    {
        PORT_LED_O &= ~(1<<BIT_LED_O);
    }
    else
    {
        PORT_LED_O |= (1<<BIT_LED_O);
    }   
}

void tmr_init(void)
{
    /* Start timer 0 with clock prescaler CLK/1024 and CTC Mode ("Clear Timer on Compare")*/
    /* Resolution is 32.25 ms */
    TCCR0 = (0<<FOC0)|(0<<WGM00)|(0<<COM01)|(0<<COM00)|(1<<WGM01)|(1<<CS02)|(1<<CS01)|(1<<CS00);

    // Reset time
    TCNT0 = 0;

    // Calculate and set period
    OCR0  = (uint16_t)(((F_RTC/1024)*PERIOD_MS)/1000) - 1;

    // Enable interrupt on compare match
    TIMSK |= (1<<OCIE0);

    // Select asynchronous timer 0 operation to use external 32.768 kHz crystal
    ASSR |= (1<<AS0);
}

int main(void)
{
    // Set LED_O as output pin
    DDR_LED_O |= (1<<BIT_LED_O);

    // Initialise timer
    tmr_init();

    // Enable interrupts
    sei();

    // Repeat indefinitely
    for(;;)
    {
        ;
    }
}

/**
 * @}
 */


