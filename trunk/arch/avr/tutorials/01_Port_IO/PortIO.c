/* ==========================================================================

   Copyright (c) 2006 Pieter Conradie [www.piconomic.co.za]
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
 * @defgroup AVR_TUT01_PORTIO /01_Port_IO
 *   
 * How to configure and use general purpose I/O pins. 
 *  
 * Files: PortIO.c 
 *  
 * The LED lights while the push button is pressed.
 *
 * Compact C techniques are used to manipulate individual bits of a Peripheral
 * Control Register. It is good programming practice to encapsulate these
 * extensively used (and error prone) pieces of code in macros, as has been
 * demonstrated in the firmware framework. The following piece of code:
 *  
 *  @code 
 *  PORT_LED_O &= ~(1<<BIT_LED_O); // Clear PB6
 *  @endcode
 *
 * should rather be replaced with a call to the following macro (defined in 
 * "board.h"):
 * 
 * @code 
 * #define LED_OFF() BIT_SET_LO(PORT_LED_O,BIT_LED_O) 
 * @endcode 
 * 
 * which uses the following macro (defined in "common.h"):
 *  
 * @code 
 * #define BIT_SET_LO(var,bit)  {var &= ~(1<<bit);}
 * @endcode 
 * 
 * @see COMMON 
 *  
 * The register and bit names are defined in
 * "C:\WinAVR\avr\include\avr\iom128.h" (assuming that WinAVR is installed in
 * it's default location). 
 *  
 * @{
 * 
 */

#include <avr/io.h>

// Input Port pin SWITCH_IP
#define PORT_SWITCH_IP  PORTB
#define DDR_SWITCH_IP   DDRB
#define PIN_SWITCH_IP   PINB
#define BIT_SWITCH_IP   5

// Output Port pin LED_O
#define PORT_LED_O      PORTB
#define DDR_LED_O       DDRB
#define BIT_LED_O       6

int main(void)
{
    // Set LED_O as output pin
    DDR_LED_O |= (1<<BIT_LED_O);

    // Set SWITCH_IP as input pin
    DDR_SWITCH_IP &= ~(1<<BIT_SWITCH_IP);

    // Enable pull-up on SWITCH_IP
    PORT_SWITCH_IP |= (1<<BIT_SWITCH_IP);

    // Repeat indefinitely
    for(;;)
    {
        // See if switch is pressed
        if((PIN_SWITCH_IP&(1<<BIT_SWITCH_IP)) == 0)
        {
            // Enable LED
            PORT_LED_O |= (1<<BIT_LED_O);
        }
        else
        {
            // Disable LED
            PORT_LED_O &= ~(1<<BIT_LED_O);
        }
    }
}

/**
 * @}
 */
