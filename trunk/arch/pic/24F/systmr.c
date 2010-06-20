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
    
    Title:          System Timer
    Author(s):      Pieter.Conradie
    Creation Date:  2010/04/12
    Revision Info:  $Id: $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "systmr.h"
#include "rtc.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static volatile systmr_ticks_t systmr_tick_counter;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/**
 *  Timer 1 interrupt handler.
 */
void __attribute__((__interrupt__,auto_psv)) _T1Interrupt(void)
{
    // Increment counter
    systmr_tick_counter++;

    // Update RTC
    rtc_on_systmr_tick();

    // Reset timer interrupt flag
    IFS0bits.T1IF = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void systmr_init(void)
{
    T1CONBITS t1conbits = T1CONbits;

    t1conbits.TCS    = 0; // Clock source: Internal clock (FOSC/2)
    t1conbits.TSYNC  = 0;
    t1conbits.TCKPS0 = 0;
    t1conbits.TCKPS1 = 0; // 1:1 prescaler
    t1conbits.TGATE  = 0; // Gated time accumulation disabled
    t1conbits.TSIDL  = 0; // Continue module operation in Idle mode
    t1conbits.TON    = 1; // Starts 16-bit Timer1
    
    T1CONbits = t1conbits;

    // Set highest priority for system timer interrupt
    IPC0bits.T1IP = 7;

    // Clear timer counter
    TMR1 = 0;

    // Set prescaler so that timer will expire 1/SYSTMR_TICKS_PER_SEC
    PR1  = DIV_ROUND(F_CY, SYSTMR_TICKS_PER_SEC) - 1;

    // Clear timer interrupt flag
    IFS0bits.T1IF = 0;

    // Enable timer interrupt
    IEC0bits.T1IE = 1;
}

systmr_ticks_t systmr_get_tick_counter(void)
{
    systmr_ticks_t counter;

	// Disable timer interrupt
    //IEC0bits.T1IE = 0;

    // Fetch counter value
    counter = systmr_tick_counter;

	// Enable timer interrupt
    //IEC0bits.T1IE = 1;

    return counter;
}

/* _____LOG__________________________________________________________________ */
/*

 2010/04/12 : Pieter.Conradie
 - Created
   
*/
