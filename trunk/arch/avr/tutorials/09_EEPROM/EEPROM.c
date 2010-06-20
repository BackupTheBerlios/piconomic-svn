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
 * @defgroup AVR_TUT09_EEPROM /09_EEPROM
 *  
 * How to write to and read from EEPROM.
 * 
 * Files: EEPROM.c
 * 
 * 0xAB is written to and then read from EEPROM address 0x000C and reported 
 * over USART0 @ 115200B, 8D, NP, 1S (RS-232 port). Then 0xCD is written to 
 * and then read from EEPROM address 0x000C and reported.
 * 
 * @note
 * It is recommended to use "C:\WinAVR\avr\include\avr\eeprom.h"
 * (assuming that WinAVR is installed in it's default location).
 * 
 * It is not neccessary to use an explicit EEPROM address. The compiler will 
 * allocate a unique address if the EEMEM prefix is used. Here is a usage example:
 * 
 * @code
 *
 *  #include <avr/eeprom.h>
 *  
 *  EEMEM unsigned short  ee_setting;
 *   
 *  int main(void)
 *  {
 *      unsigned short setting;
 * 
 *      // Load setting from EEPROM
 *      setting = eeprom_read_word(&ee_setting);
 *   
 *      // ...
 *  
 *      // Commit new setting to EEPROM
 *      eeprom_write_word(&ee_setting, setting);
 *      eeprom_busy_wait();
 *  } 
 * @endcode
 * 
 * @{
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

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

    // Route STDOUT stream to printf_stream
    stdout = &printf_stream;
}

uint8_t ee_read_byte(uint16_t address)
{
    // Wait until EEPROM is ready
    while(EECR&(1<<EEWE))
    {
        ;
    }
    // Set address
    EEARL = address&0xff;
    EEARH = (address>>8)&0xff;
    // Start read operation
    EECR |= (1<<EERE);
    // Return data
    return EEDR;
}

void ee_write_byte(uint16_t address, uint8_t data)
{
    uint8_t sreg;

    // Wait until EEPROM is ready
    while(EECR&(1<<EEWE))
    {
        ;
    }
    // Set address
    EEARL = address&0xff;
    EEARH = (address>>8)&0xff;
    // Set data to write
    EEDR = data;
    // Save status of global interrupts
    sreg = SREG;
    // Disable interrupts
    cli();
    // Start write operation
    EECR |= (1<<EEMWE);
    EECR |= (1<<EEWE);
    // Restore status of global interrupts
    SREG = sreg;   
}
int main(void)
{
    uint8_t data;

    // Initialise PRINTF functionality
    printf_init();

    PRINTF("\nEEPROM Example\n");

    // Write a byte to EEPROM
    ee_write_byte(0x000C,0xAB);

    // Read a byte from EEPROM
    data = ee_read_byte(0x000C);

    PRINTF("EEPROM value @ 0x000C is 0x%02X\n",data);

    // Write a byte to EEPROM
    ee_write_byte(0x000C,0xCD);

    // Read a byte from EEPROM
    data = ee_read_byte(0x000C);

    PRINTF("EEPROM value @ 0x000C is 0x%02X\n",data);

    // Repeat indefinitely
    for(;;)
    {
        ;
    }
}

/**
 * @}
 */
