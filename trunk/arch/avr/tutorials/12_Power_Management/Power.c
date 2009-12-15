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
 * @defgroup AVR_TUT12_POWER_MANAGEMENT /12_Power_Management
 *  
 * How to use the asynchronous timer 0 and the 32.768 kHz crystal to achieve a deeper sleep mode called "POWER-SAVE".
 * 
 * Files: 
 * - Power.c 
 * - general/common.h 
 * - arch/avr/boards/piconomic_ab111-4/board.h
 * 
 * The AVR wakes up every 50 ms and "mirrors" the state of the push button to 
 * the LED. The AVR will also wake up asynchronously on a rising or falling edge
 * interrupt on PD0 (INT0/SCL) and output the state of PD0 to PD1 (INT1/SDA). 
 * This feature can be used to measure the responsiveness of the AVR using a 
 * signal generator and an oscilloscope.
 * 
 * The main 7.3728 MHz crystal is disabled when the AVR enters "POWER-SAVE" 
 * sleep mode. When the asynchronous timer 0 interrupt or external interrupt 0 
 * wakes up the AVR, the crystal takes some time to oscillate at the proper 
 * frequency.
 * 
 * A caveat of the AVR is that the main crystal will not be disabled while the 
 * JTAG interface is enabled. It can be disabled by programming the AVR fuse 
 * bits, or using a specific sequence of code, which is included in the tutorial.
 * This will also be necessary when ADC4-7 of the ADC converter is needed 
 * (which is shared with the JTAG interface). 
 * 
 * @{
 * 
 */

#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "common.h"
#include "board.h"

#define TMR0_PERIOD_MS      50
#define TMR0_TCCR0_VALUE    ((0<<FOC0)|(0<<WGM00)|(0<<COM01)|(0<<COM00)|(1<<WGM01)|(0<<CS02)|(1<<CS01)|(0<<CS00))

// Interrupt handler on external interrupt 0
ISR(INT0_vect)
{
    // Toggle edge
    if(BIT_IS_LO(EICRA,ISC00))
    {
        // Change interrupt to rising edge
        BIT_SET_HI(EICRA,ISC00);
    }
    else
    {
        // Change interrupt to falling edge
        BIT_SET_LO(EICRA,ISC00);
    }

    // Mirror input pin to output
    if(BIT_IS_HI(PIND,PD0))
    {
        // Set output pin high
        BIT_SET_HI(PORTD,PD1);
    }
    else
    {
        // Set output pin low
        BIT_SET_LO(PORTD,PD1);
    }
}

// Interrupt handler on compare match (TCNT0 is cleared automatically)
ISR(TIMER0_COMP_vect)
{
    // Do nothing. This interrupt ensures that main loop 
    // is executed at least every 50 ms (TMR0_PERIOD_MS)
}

void tmr0_init(void)
{
    // Select asynchronous timer 0 operation to use external 32.768 kHz crystal
    BIT_SET_HI(ASSR,AS0);

    // Reset time
    TCNT0 = 0;

    // Calculate and set period
    OCR0 = (uint16)DIV(((F_RTC/8)*TMR0_PERIOD_MS),1000) - 1;

    /* Start timer 0 with clock prescaler CLK/8 and CTC Mode ("Clear Timer on Compare")*/
    /* Resolution is 244.14 us */
    /* Maximum period is 62.5 ms */
    TCCR0 = TMR0_TCCR0_VALUE;

    // Wait for "update busy" flags to clear
    while(ASSR&((1<<TCN0UB)|(1<<OCR0UB)|(1<<TCR0UB)))
    {
        ;
    }

    // Enable interrupt on compare match
    BIT_SET_HI(TIMSK,OCIE0);
}

void TMR0_vWaitOneTOSC1(void)
{
    // Do "dummy" write to TCCR0 register for TOSC1 timed transfer
    TCCR0 = TMR0_TCCR0_VALUE;
    // Wait for "update busy" flag to clear
    LOOP_UNTIL_BIT_IS_LO(ASSR,TCR0UB);
}

int main(void)
{
    // Initialise IO pins
    PORTB =  (1<<BIT_SPI_SS_O)|(1<<BIT_FLASH_CS_O)|(1<<BIT_SW_IP)
            |(0<<BIT_LED_O)|(1<<BIT_BUZZER_O);
    DDRB  =  (1<<BIT_SPI_SS_O)|(1<<BIT_FLASH_CS_O)|(0<<BIT_SW_IP)
            |(1<<BIT_LED_O)|(1<<BIT_BUZZER_O);

    PORTD =  (0<<BIT_RS485_TX_EN_O)|(1<<BIT_RS485_RX_EN_O)
            |(0<<BIT_RTS_I)|(0<<BIT_CTS_O);
    DDRD  =  (1<<BIT_RS485_TX_EN_O)|(1<<BIT_RS485_RX_EN_O)
            |(0<<BIT_RTS_I)|(1<<BIT_CTS_O);

    // Initialise modules
    tmr0_init();

    // Enable external interrupt 0 on falling edge
    EICRA =  (0<<ISC31)|(0<<ISC30)|(0<<ISC21)|(0<<ISC20)
            |(0<<ISC11)|(0<<ISC10)|(1<<ISC01)|(0<<ISC00);
    EIMSK =  (0<<INT7)|(0<<INT6)|(0<<INT5)|(0<<INT4)
            |(0<<INT3)|(0<<INT2)|(0<<INT1)|(1<<INT0);

    // Select sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);

    // Disable On Chip Debug System, otherwise main clock source stays enabled
    // See p.48 of ATmega128 datasheet
    BIT_SET_HI(MCUCSR,JTD);
    // This bit must be set twice within four cycles to change its value.
    // See p.258 of ATmega128 datasheet
    BIT_SET_HI(MCUCSR,JTD);

    // Disable Analog Comparator to reduce power consumption
    BIT_SET_HI(ACSR,ACD);

    // Enable interrupts
    sei();

    // Loop indefinitely
    for(;;)
    {
        // Wait at least one TOSC1 cycle for interrupt logic to reset,
        // otherwise the device will fail to wake up when sleep mode is entered
        TMR0_vWaitOneTOSC1();

        // Go into sleep mode until an event wakes up the CPU
        sleep_mode();

        // See if switch is pressed
        if(SW_IS_PRESSED())
        {
            LED_ON();
        }
        else
        {
            LED_OFF();
        }
    }
}

/**
 * @}
 */
