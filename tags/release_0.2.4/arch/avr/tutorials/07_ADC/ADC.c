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
 * @defgroup AVR_TUT07_ADC /07_ADC
 *  
 * How to configure and use one channel of the ADC converter in a polled fashion.
 * 
 * Files: 07_ADC/ADC.c
 * 
 * The analogue voltage of AD1 (Pin 60 - PF1) is measured once a second and 
 * reported via USART0 @ 115200B, 8D, NP, 1S (RS-232 port). A voltage between 
 * +0V to +3.3V may be applied.
 * 
 * @note
 * The floating point support of the printf library is enabled in the Makefile with:
 * @code
 * PRINTF_LIB = $(PRINTF_LIB_FLOAT) 
 * @endcode
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
    // Set baud rate
    UBRR0H = (uint8_t)(USART0_UBBR_VALUE>>8);
    UBRR0L = (uint8_t)USART0_UBBR_VALUE;

    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C = (0<<USBS)|(1<<UCSZ1)|(1<<UCSZ0);

    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN)|(1<<TXEN);   
}

int usart0_put_char(char cData, FILE* pxStream)
{
    // Recursive function to prepend a carriage return before a new line
    if(cData == '\n')
    {
        usart0_put_char('\r',pxStream);
    }

    // Wait if a byte is being transmitted
    while((UCSR0A&(1<<UDRE0)) == 0)
    {
        ;
    }

    // Transmit data
    UDR0 = cData;

    return 0;
}

// Create PRINTF Stream structure
FILE printf_stream = FDEV_SETUP_STREAM(usart0_put_char, NULL, _FDEV_SETUP_WRITE);

void tmr_init(void)
{
    /* Start timer 1 with clock prescaler CLK/1024 */
    /* Resolution is 139 us */
    /* Maximum time is 9.1 s */
    TCCR1A =  (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)
             |(0<<COM1C1)|(0<<COM1C0)|(0<<WGM11) |(0<<WGM10);

    TCCR1B =  (0<<ICNC1) |(0<<ICES1) |(0<<WGM13) |(0<<WGM12)
             |(1<<CS12)  |(0<<CS11)  |(1<<CS10);
}

void tmr_delay(uint16_t u16DelayMs)
{
    // Calculate and set delay
    TCNT1  = (uint16_t) (0x10000 - ((F_CPU/1024)*u16DelayMs)/1000);

    // Clear timer overflow flag
    TIFR = (1<<TOV1);

    // Wait until timer overflow flag is set
    while((TIFR&(1<<TOV1)) == 0)
    {
        ;
    }
}

void adc_init(void)
{
    /* 
       Select AVCC as reference with external capacitor at AREF pin
       ADC1 as the single-ended input channel with 1x gain
    */
    ADMUX =  (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)
            |(0<<MUX4) |(0<<MUX3) |(0<<MUX2) |(0<<MUX1)|(1<<MUX0);

    /*
       Enable ADC; Select Prescaler of 128 (clock frequency of 57.6 kHz)
    */
    ADCSRA =  (1<<ADEN)|(0<<ADSC) |(0<<ADFR) |(0<<ADIF)
             |(0<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t adc_get_sample(void)
{
    // Start conversion
    ADCSRA |= (1<<ADSC);

    // Wait until conversion is finished... 13 ADC clock cycles
    // 4.43 kSPS
    while(ADCSRA&(1<<ADSC))
    {
        ;
    }

    // Return sampled value
    return ADC;
}

int main(void)
{
    uint16_t u16AdcValue;
    float    fAdcVoltage;

    // Initialise USART
    usart0_init();

    // Route STDOUT stream to PRINTF_xStream
    stdout = &printf_stream;

    // Initialise timer
    tmr_init();

    // Initialise ADC
    adc_init();

    // Repeat indefinitely
    for(;;)
    {
        // Retrieve a sample
        u16AdcValue = adc_get_sample();

        // Calculate voltage
        fAdcVoltage = (((float)u16AdcValue)/1023)*3.3;
        // Output value
        PRINTF("ADC Value is %d\n",u16AdcValue);
        PRINTF("ADC Voltage is %1.2fV\n",fAdcVoltage);

        // Wait 1 second
        tmr_delay(1000);
    }
}

/**
 * @}
 */
