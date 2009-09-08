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
 * @defgroup AVR_TUT02_TIMERS /02_Timers
 *  
 * How to configure and use a timer peripheral as a polled timeout/delay function. 
 * 
 * Files: Timers.c
 * 
 * The buzzer beeps repeatedly for 100 ms with a 1s delay in between.
 *    
 * One of the goals was to keep the tutorials small, compact and self-contained.
 * Thus a good programming practice to encapsulate a functional block in a 
 * separate H and C file was ignored, e.g. all the "tmr_xxx(...)" functions 
 * should be kept in a "tmr.h" and "tmr.c" file.
 * 
 * @see TMR
 * 
 * @{
 * 
 */

#include <stdint.h>
#include <avr/io.h>

// Boolean definitions
typedef enum
{
    FALSE = 0,
    TRUE  = !FALSE,
} bool_t;

// Output Port pin BUZZER_O
#define PORT_BUZZER_O PORTB
#define DDR_BUZZER_O  DDRB
#define BIT_BUZZER_O  7

void tmr_init(void)
{
    /* Start timer 1 with clock prescaler CLK/1024 */
    /* Resolution is 139 us */
    /* Maximum time is 9.1 s */
    TCCR1A =  (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)
             |(0<<COM1C1)|(0<<COM1C0)|(0<<WGM11) |(0<<WGM10);

    TCCR1B =  (0<<ICNC1) |(0<<ICES1) |(0<<WGM13) |(0<<WGM12)
             |(1<<CS12)  |(0<<CS11)  |(1<<CS10);
}

void tmr_start(uint16_t delay_ms)
{
    // Calculate and set delay
    TCNT1  = (uint16_t) (0x10000 - ((F_CPU/1024)*delay_ms)/1000);

    // Clear timer overflow flag by writing a logical 1, other flags are unchanged
    TIFR = (1<<TOV1);
    // NOTE: This line is more efficient than using "TIFR |= (1<<TOV1);"
}

bool_t tmr_has_expired(void)
{
    // See if timer overflow flag is set
    if(TIFR&(1<<TOV1))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void tmr_delay(uint16_t delay_ms)
{
    tmr_start(delay_ms);
    while(!tmr_has_expired())
    {
        ;
    }
}

void beep(void)
{
    // Enable Buzzer
    PORT_BUZZER_O &= ~(1<<BIT_BUZZER_O);

    // Wait 100 ms
    tmr_delay(100);

    // Disable Buzzer
    PORT_BUZZER_O |= (1<<BIT_BUZZER_O);
}

int main(void)
{
    // Set BUZZER_O as output pin
    DDR_BUZZER_O |= (1<<BIT_BUZZER_O);

    // Disable Buzzer
    PORT_BUZZER_O |= (1<<BIT_BUZZER_O);

    // Initialise timer
    tmr_init();

    // Repeat indefinitely
    for(;;)
    {
        beep();   

        // Wait 1 s
        tmr_delay(1000);
    }
}

/** 
 * @}
 */
