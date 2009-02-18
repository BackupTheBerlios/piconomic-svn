/* =============================================================================

    Copyright (c) 2009 Pieter Conradie <pieterconradie@users.berlios.de>
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
    
    Title:          ARM AT91 LED Blink example
    Author(s):      Pieter.Conradie
    Creation Date:  2009-02-17
    Revision Info:  $Id$
 
============================================================================= */

/** 
 * @ingroup ARM_AT91_EXAMPLES
 * @defgroup ARM_AT91_LED_BLINK /led_blink
 * 
 * This example toggles the LED once a second.
 *  
 * The following modules are exercised: 
 * - @ref AT91_PITD 
 * - @ref TMR
 * 
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
#include "pitd.h"
#include "tmr.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// LED is on PIOA, pin 0
#define BASE_LED_PIO    AT91C_BASE_PIOA
#define PIN_LED_O       0

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static void led_init  (void);
static void led_on    (void);
static void led_off   (void);
static void led_toggle(void);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void led_init(void)
{
    // Enable the PIO to control the corresponding pin
    BASE_LED_PIO->PIO_PER  = (1<<PIN_LED_O);

    // Enable the output on the I/O line
    BASE_LED_PIO->PIO_OER  = (1<<PIN_LED_O);

    // Set initial state of LED to on
    led_on();    
}

static void led_on(void)
{
    // Set the data to be driven on the I/O line
    BASE_LED_PIO->PIO_SODR = (1<<PIN_LED_O);
}

static void led_off(void)
{
    // Clear the data to be driven on the I/O line
    BASE_LED_PIO->PIO_CODR = (1<<PIN_LED_O);
}

static void led_toggle(void)
{
    // See if LED is on
    if(BASE_LED_PIO->PIO_ODSR & (1<<PIN_LED_O))
    {
        led_off();
    }
    else
    {
        led_on();
    }
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    tmr_t tmr;

    // Initialise modules
    led_init();
    pit_init();

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
        led_toggle();
    }
}

/* _____LOG__________________________________________________________________ */
/*

 2009-02-17 : Pieter.Conradie
 - First release
   
*/
