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
 * @defgroup AVR_TUT13_XMEM /13_External_Memory
 *  
 * Demonstrates how to enable the external memory interface and redirect the heap to external memory.
 * 
 * Files: XMEM.c
 * 
 * A 64-byte block of external SRAM is filled with a test pattern (first zero's,
 * then incrementing values) and reported via USART0 @ 115200B, 8D, NP, 1S 
 * (RS-232 port).
 * 
 * The linker must be informed that external memory must be used for the heap 
 * (the default location is internal SRAM, between the uninitialized variables 
 * section and the downwards growing stack). The following line in the Makefile 
 * accomplishes this:
 * 
 * @code
 * EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x8090ff
 * @endcode
 * 
 * Refer to section 8.5 "Using Malloc" of the AVR-LIBC Reference Manual for a 
 * detailed explanation.
 * 
 * It is recommended to place the function "xmem_enable()" in the ".init1" 
 * section to enable external memory early during the start up of the 
 * application, e.g.:
 * 
 * @code
 * __attribute__ ((section (".init1))) xmem_enable(void)
 * {
 *     ...
 * }
 * @endcode
 * 
 * @{
 * 
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

// Define size of memory block
#define BUFFER_SIZE 64

// Macro to send strings stored in program memory space
#define PRINTF(format, ...) printf_P(PSTR(format), ## __VA_ARGS__)

// Define baud rate
#define USART0_BAUD         115200ul
#define USART0_UBBR_VALUE   ((F_CPU/(USART0_BAUD<<4))-1)

void usart0_init(void)
{
    // Set baud rate
    UBRR0H = (uint8_t)(USART0_UBBR_VALUE>>8);
    UBRR0L = (uint8_t)USART0_UBBR_VALUE;

    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C = (0<<USBS)|(1<<UCSZ1)|(1<<UCSZ0);

    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN)|(1<<TXEN);   
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

void printf_init(void)
{
    // Initialise USART
    usart0_init();

    // Route STDOUT stream to PRINTF_xStream
    stdout = &printf_stream;
}

void xmem_enable(void)
{
    MCUCR = (1<<SRE)|(0<<SRW10);   /* External memory interface enable */
    XMCRA = (0<<SRL2)|(0<<SRL1)|(0<<SRL0)|(0<<SRW01)|(0<<SRW00)|(0<<SRW11);
    /* 
       Lower sector = N/A
       Upper sector = 0x1100 - 0xFFFF 
       No wait cycles for upper sector
    */
    XMCRB = (0<<XMBK)|(0<<XMM2)|(0<<XMM1)|(0<<XMM0);
    /* 
       External Memory Bus-keeper Disabled ;
       External Memory High Mask - no released port pins
    */
}

int main(void)
{
    uint8_t *mem;
    uint8_t index;

    // Initialise external memory interface
    xmem_enable();
    // NB: "xmem_enable()" must be called before any function that uses heap memory

    // Initialise PRINTF functionality
    printf_init();   

    PRINTF("\nExternal Memory Example\n");   

    // Allocate external memory
    mem = malloc(BUFFER_SIZE);

    PRINTF("%d Byte buffer (starting at 0x%04X) filled with 0's:\n",BUFFER_SIZE,mem);

    // Fill memory with 0's
    for(index = 0; index < BUFFER_SIZE; index++)
    {
        mem[index] = 0x00;
    }
    // Display memory block
    for(index = 0; index < BUFFER_SIZE; index++)
    {
        PRINTF("%02X ",mem[index]);
        if((index&0x0F) == 0x0F)
        {
            PRINTF("\n");
        }
    }
    PRINTF("\n");

    PRINTF("%d Byte buffer (starting at 0x%04X) filled with incrementing numbers:\n",
           BUFFER_SIZE, mem);

    // Fill memory with incrementing numbers
    for(index = 0; index < BUFFER_SIZE; index++)
    {
        mem[index] = index;
    }
    // Display memory block
    for(index = 0; index < BUFFER_SIZE; index++)
    {
        PRINTF("%02X ", mem[index]);
        if((index&0x0F) == 0x0F)
        {
            PRINTF("\n");
        }
    }
    PRINTF("\n");

    // Repeat indefinitely
    for(;;)
    {
        ;
    }
}

/**
 * @}
 */


