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
    
    Title:          Periodic Interval Timer using Timer 1
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: pit_tmr1.c 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <avr/interrupt.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "pit_tmr1.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static volatile pit_ticks_t pit_tick_counter;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
/**
 *  Timer compare match interrupt handler.
 */
ISR(TIMER1_COMPA_vect)
{
    // Increment counter
    pit_tick_counter++;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void pit_init(void)
{
    // Initialise timer 1 interrupt @ CLK/8 in CTC mode ("Clear Timer on Compare")
    // Resolution is 1.085 us
    TCCR1A =  (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)
             |(0<<COM1C1)|(0<<COM1C0)|(0<<WGM11) |(0<<WGM10);

    TCCR1B =  (0<<ICNC1)|(0<<ICES1)|(0<<WGM13)|(1<<WGM12)
             |(0<<CS12) |(1<<CS11) |(0<<CS10);

    // Set 16-bit TMR1 counter to 0
    TCNT1  = 0;

    // Calculate and set maximum 16-bit TMR1 counter value.
    // When TCNT1 reaches this value, an interrupt is generated
    // and TCNT1 is reset to 0.
    OCR1A  = DIV_ROUND(F_CPU/8, PIT_TICKS_PER_SEC)-1;

    // Enable Timer compare match interrupt
    BIT_SET_HI(TIMSK, OCIE1A);
}

pit_ticks_t pit_get_counter(void)
{
    pit_ticks_t tsTickCounter;

    // Disable timer interrupt to perform atomic access
    BIT_SET_LO(TIMSK,OCIE1A);

    // Fetch current time
    tsTickCounter = pit_tick_counter;

    // Enable timer interrupt
    BIT_SET_HI(TIMSK,OCIE1A);

    return tsTickCounter;
}

/**
 *  @}
 */

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : PJC
 - First release
   
*/
