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

    Title:          PRINTF module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "printf.h"
#include "uart0.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static int printf_put_char(char data, FILE *stream);

/* _____LOCAL VARIABLES______________________________________________________ */
/// Declare stream object in data memory (not heap memory)
FILE printf_stream = FDEV_SETUP_STREAM(printf_put_char, NULL, _FDEV_SETUP_WRITE);

/* _____LOCAL FUNCTIONS______________________________________________________ */
/**
 *  Function to send a byte.
 *  
 *  Every carriage return ("\n") will be intercepted and replaced with a
 *  carriage return, new line sequence ("\r\n").
 */
static int printf_put_char(char data, FILE *stream)
{
    // Recursive function to prepend a carriage return before a new line character
    if(data == '\n')
    {
        printf_put_char('\r',stream);
    }

    // Send character over UART0 and make sure it is buffered
    while(!uart0_tx_byte((u8_t)data))
    {
        ;
    }   

    return 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void printf_init(void)
{
    // Route stdout stream to printf_stream
    stdout = &printf_stream;
}

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : PJC
 - First release
   
*/
