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
 * @defgroup AVR_TUT06_PWM /06_PWM
 *  
 * How to configure and use a timer peripheral to generate a PWM output.
 * 
 * Files: PWM.c
 * 
 * The LED flashes at a rate of 450 Hz with a 1% duty cycle. This will not be 
 * observable by the human eye.
 * 
 * @note
 * An undesirable side-effect effect of the flexibility and power of the timer 
 * peripherals is the sheer size of the datasheet documentation. Most times it 
 * helps to skip the lengthy description and focus on the actual peripheral 
 * register description, e.g. do not start at p.111, but jump directly to 
 * "16-bit Timer/Counter Register Description" p.133 of the ATmega128 datasheet.
 * 
 * @{
 * 
 */

#include <stdint.h>
#include <avr/io.h>

// Output Port pin LED_O
#define PORT_LED_O         PORTB
#define DDR_LED_O          DDRB
#define BIT_LED_O          6

// Define frequency
#define PWM_MAX_DUTY_CYCLE 0x3FF

void pwm_init(void)
{
    /* 
       Start Timer 1 with clock prescaler CLK/8 and phase correct 
       10-bit PWM mode. Output on PB6 (OC1B). Resolution is 1.09 us.
       Frequency is 450 Hz.
    */
    TCCR1A =  (0<<COM1A1)|(0<<COM1A0)|(1<<COM1B1)|(0<<COM1B0)
             |(0<<COM1C1)|(0<<COM1C0)|(1<<WGM11) |(1<<WGM10);

    TCCR1B =  (0<<ICNC1) |(0<<ICES1) |(0<<WGM13) |(0<<WGM12)
             |(0<<CS12)  |(1<<CS11)  |(0<<CS10);

    // Reset counter
    TCNT1 = 0;

    // Set duty cycle to 0%
    OCR1B  = 0;
}

void pwm_set_duty_cycle(uint16_t duty_cycle)
{
    // Clip parameter to maximum value
    if (duty_cycle > PWM_MAX_DUTY_CYCLE)
    {
        duty_cycle = PWM_MAX_DUTY_CYCLE;
    }

    OCR1B = duty_cycle;
}

int main(void)
{
    // Set LED_O as output pin
    DDR_LED_O |= (1<<BIT_LED_O);

    // Initialise PWM
    pwm_init();

    // Set duty cycle to 1%
    pwm_set_duty_cycle(PWM_MAX_DUTY_CYCLE/100);

    // Repeat indefinitely
    for(;;)
    {
        ;
    }
}

/**
 * @}
 */
