/* =============================================================================

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

    Title:          Simple polled ADC Driver
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "adc.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void adc_init(void)
{
    /* 
       Select AVCC as reference with external capacitor at AREF pin
       and ADC0 as the single-ended input channel with 1x gain.
    */
    ADMUX =  (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)
            |(0<<MUX4) |(0<<MUX3) |(0<<MUX2) |(0<<MUX1)|(0<<MUX0);

    /*
       Enable ADC; Select Prescaler of 128 (clock frequency of 57.6 kHz)
    */
    ADCSRA =  (1<<ADEN)|(0<<ADSC) |(0<<ADFR) |(0<<ADIF)
             |(0<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

void adc_sel_channel(const u8_t channel)
{
    // Select new ADC channel in one atomic operation
    u8_t admux = ADMUX;

    admux &= ~((1<<MUX4)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));
    admux |= (channel<<MUX0);

    ADMUX = admux;
}

u16_t adc_get_sample(void)
{
    // Start conversion
    BIT_SET_HI(ADCSRA, ADSC);

    // Wait until conversion is finished... 13 ADC clock cycles
    // 4.43 kSPS
    LOOP_UNTIL_BIT_IS_LO(ADCSRA, ADSC);

    // Return sampled value
    return ADC;
}

u32_t adc_get_samples(const u8_t channel, u8_t nr_of_samples)
{
    u32_t value = 0;

    // Select the specified channel
    adc_sel_channel(channel);

    // Repeat until all the samples have been taken
    while(nr_of_samples != 0)
    {
        value += adc_get_sample();
        nr_of_samples--;
    }

    // Return the accumulated value
    return value;
}

/**
 *  @}
 */

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : Pieter Conradie
 - First release
   
*/
