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
 * @defgroup AVR_TUT04_USARTS /04_USARTs
 *  
 * How to configure and use a USART in a polled fashion.
 * 
 * Files: USARTs.c 
 *  
 * USART0 (RS-232 port) is configured to 115200 Baud, 8 Data Bits, No Parity 
 * and 1 Stop Bit. The string "AVR\r\n" is sent upon start up. Thereafter all 
 * received characters are echoed back to the sender.
 * 
 * The clock rate of the AVR is defined in the Makefile with the macro name 
 * @b F_CPU. It is a good programming practice to calculate values at compile 
 * time, in stead of using "magic values", e.g. the value of UBBR, which is set
 * with the following macro:
 * 
 * @code
 * #define UART0_UBBR_VALUE  ((F_CPU/(UART0_BAUD<<4))-1)
 * @endcode
 * 
 * in stead of
 * @code
 * #define UART0_UBBR_VALUE   3 // Magic Value for 115200 Baud at a clock rate of 7.3728 MHz 
 * @endcode
 * 
 * @{
 * 
 */

#include <stdint.h>
#include <avr/io.h>

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

void usart0_send_byte(uint8_t data)
{
    // Wait if a byte is being transmitted
    while((UCSR0A&(1<<UDRE0)) == 0)
    {
        ;
    }

    // Transmit data
    UDR0 = data; 
}

uint8_t usart0_receive_byte()
{
    // Wait until a byte has been received
    while((UCSR0A&(1<<RXC0)) == 0)
    {
        ;
    }

    // Return received data
    return UDR0;
}

int main(void)
{
    uint8_t data;

    // Initialise USART
    usart0_init();

    // Send string
    usart0_send_byte('A');
    usart0_send_byte('V');
    usart0_send_byte('R');
    usart0_send_byte('\r');
    usart0_send_byte('\n');

    // Repeat indefinitely
    for(;;)
    {
        // Echo received characters
        data = usart0_receive_byte();
        usart0_send_byte(data);
    }
}

/**
 * @}
 */
