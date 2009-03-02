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
 * @defgroup AVR_TUT03_INTERRUPTS /03_Interrupts
 *  
 * How to implement a simple interrupt handler.
 * 
 * A timer is configured to generate an interrupt every 250 ms. The LED output 
 * pin is toggled every 250 ms in the timer interrupt.
 * 
 * A macro sei() is called to enable interrupts globally. It sets bit 7 (I) of 
 * the SREG register. cli() can be called to disable all interrupts.
 * 
 * Refer to "Interrupt Vectors in ATmega128" p.59 of the ATmega128 datasheet 
 * to view the list of interrupt vectors.
 * 
 * The vector names are also defined in "C:\WinAVR\avr\include\avr\iom128.h" 
 * (assuming that WinAVR is installed in it's default location). 
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

// Define delay
#define PERIOD_MS       250

// Interrupt handler on compare match (TCNT1 is cleared automatically)
ISR(TIMER1_COMPA_vect)
{
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
    /*
     * Start timer 1 with clock prescaler CLK/1024 and CTC Mode ("Clear Timer on Compare")
     *
     * Resolution is 139 us
     * Maximum time is 9.1 s
     */
    TCCR1A =  (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)
             |(0<<COM1C1)|(0<<COM1C0)|(0<<WGM11) |(0<<WGM10);

    TCCR1B =  (0<<ICNC1) |(0<<ICES1) |(0<<WGM13) |(1<<WGM12)
             |(1<<CS12)  |(0<<CS11)  |(1<<CS10);

    // Reset time
    TCNT1 = 0;

    // Calculate and set delay
    OCR1A  = (uint16_t)(((F_CPU/1024)*PERIOD_MS)/1000) - 1;

    // Enable interrupt on compare match
    TIMSK |= (1<<OCIE1A);
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
