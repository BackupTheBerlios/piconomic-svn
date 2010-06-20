/* =============================================================================

    Copyright (c) 2008 Pieter Conradie [www.piconomic.co.za]
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
    
    Title:          Atmel AVR LED Blink example
    Author(s):      Pieter.Conradie
    Creation Date:  2007-04-27
    Revision Info:  $Id$
 
============================================================================= */

/** 
 * @ingroup AVR_EXAMPLES
 * @defgroup AVR_LED_BLINK /led_blink
 * 
 * This example outputs a string on the serial port and toggles the LED once a 
 * second. 
 *  
 * The following modules are exercised: 
 * - @ref AVR_SYSTMR 
 * - @ref PRINTF_MODULE 
 * - @ref UART0 
 * - @ref TMR 
 * - @ref PICONOMIC_AB111_4
 * 
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/interrupt.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
#include "systmr.h"
#include "printf.h"
#include "uart0.h"
#include "tmr.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    tmr_t tmr;

    // Initialise the board
    board_lowlevel_init();

    // Enable LED
    LED_ON();

    // Initialise modules
    uart0_init(115200, 8, UART0_NO_PARITY, 1);
    printf_init();
    systmr_init();

    // Enable interrupt
    sei();

    PRINTF("Atmel AVR LED Blink example\n");

    // Initialise timer to expire once a second
    tmr_start(&tmr, TMR_TICKS_PER_SEC);

    for(;;)
    {
        // Wait until timer has expired
        while(!tmr_has_expired(&tmr))
        {
            ;
        }
        // Reset timer
        tmr_reset(&tmr);

        // Toggle LED pin
        LED_TOGGLE();
    }
}

/* _____LOG__________________________________________________________________ */
/*

 2009-02-17 : Pieter.Conradie
 - First release
   
*/
