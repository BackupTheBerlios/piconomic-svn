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
 * @defgroup AVR_TUT05_PRINTF /05_printf
 *  
 * How to direct the AVR-LIBC stdout stream to send characters using a USART.
 * 
 * Files: 05_printf/Printf.c 
 *  
 * USART0 (RS-232 port) is configured to 115200B, 8D, NP, 1S. The following 
 * strings are sent upon start up:
 * - "printf - String in data memory (RAM)"
 * - "printf_P - String in program memory (FLASH)"
 * - "PRINTF - String in program memory (FLASH)"
 * 
 * A custom handler "usart0_put_char(...)" is linked to the stdout stream to 
 * send a character and inject a "\r" before every "\n".
 * 
 * This example draws attention to the handling of the Harvard architecture by 
 * the GCC compiler and the AVR-LIBC library. The 8-bit AVRs have a small amount
 * of SRAM (data memory) and therefore it is desirable to store constant strings 
 * in FLASH (program memory) and access it from there, not using a copy in SRAM. 
 * The recommended solution is to use the macros in 
 * "C:\WinAVR\avr\include\avr\pgmspace.h" and string handling functions with the
 * suffix "_P", e.g. "printf_P(...)" in stead of "printf(...)". This handling has
 * been encapsulated with the following variable argument macro:
 * 
 * @code
 * #define PRINTF(format, ...) printf_P(PSTR(format), ## __VA_ARGS__)
 * @endcode
 * 
 * @see PRINTF_MODULE
 * 
 * @{
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

// Macro to send strings stored in program memory space
#define PRINTF(format, ...) printf_P(PSTR(format), ## __VA_ARGS__)

// Define baud rate
#define USART0_BAUD         115200ul
#define USART0_UBBR_VALUE   ((F_CPU/(USART0_BAUD<<4))-1)

void usart0_init(void)
{
    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C = (0<<USBS)|(1<<UCSZ1)|(1<<UCSZ0);

    // Set baud rate
    UBRR0H = (uint8_t)(USART0_UBBR_VALUE>>8);
    UBRR0L = (uint8_t)USART0_UBBR_VALUE;   

    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);   
}

int usart0_put_char(char data, FILE *stream)
{
    // Recursive function to prepend a carriage return before a new line
    if(data == '\n')
    {
        usart0_put_char('\r',stream);
    }

    // Wait if a byte is being transmitted
    while((UCSR0A&(1<<UDRE0)) == 0)
    {
        ;
    }

    // Transmit data
    UDR0 = data;

    return 0;
}

// Create PRINTF Stream structure
FILE printf_stream = FDEV_SETUP_STREAM(usart0_put_char, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
    // Initialise USART
    usart0_init();

    // Direct stdout stream to printf_stream
    stdout = &printf_stream;

    // Send strings
    printf("printf - String in data memory (RAM)\n");

    printf_P(PSTR("printf_P - String in program memory (FLASH)\n"));

    PRINTF("PRINTF - String in program memory (FLASH)\n");

    // Repeat indefinitely
    for(;;)
    {
        ;
    }
}

/**
 * @}
 */
