#ifndef __ADC_H__
#define __ADC_H__
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

    Title:          Simple polled ADC Driver
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: adc.h 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */
/** 
 *  @ingroup AVR
 *  @defgroup ADC_SIMPLE adc.h : Simple polled ADC Driver
 *
 *  Simple polled ADC peripheral driver to sample channels ADC0 to ADC7.
 *
 *  Files: adc.h & adc.c
 *
 *  The ADC peripheral is initialised in ADC_vInit(). The AVCC pin is selected
 *  as the full range reference, as well as a gain of 1. This means that if 
 *  AVCC is +3.3V, then the digital range 0 to 1023 (10 bits) will correspond 
 *  to +0V to +3.297V and the resolution will be 3.223 mV:
 *
 *  ADC = (Vin * 1024) / Vref
 *
 *  The ADC clock frequency is configured to 57.6 kHz (for an external crystal
 *  of @b 7.3728 @b MHz; ADC clock prescaler = 128). When a new channel is 
 *  selected with adc_sel_channel(), the first sample with adc_get_sample() will
 *  take 25 ADC clock cycles (434 us) to complete, because the the analog 
 *  circuitry is also initialised (calibrated). Thereafter each successive 
 *  sample on the same channel will take 13 ADC clock cycles (226 us).
 *
 *  @par Example:
 *  @include adc_test.c
 *  
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise ADC peripheral.
 * 
 */
extern void adc_init(void);

/**
 *  Select ADC Channel.
 * 
 *  @note No error check is performed that an illegal u8Channel value is specified.
 * 
 *  @param[in] channel      0 to 7 corresponds to ADC0 to ADC7
 */
extern void adc_sel_channel(const u8_t channel);

/**
 *  Start conversion and wait until finished.
 * 
 *  It will take 25 ADC clock cycles to complete the first time a 
 *  channel is selected, therafter it will take 13 ADC clock cycles.
 * 
 *  @return u16_t   10-bit ADC value (0 to 1023)
 */
extern u16_t adc_get_sample(void);

/**
 *  Select ADC channel and accumulate number of specified samples.
 * 
 *  It will take 25 + (13 * nr_of_samples) ADC clock cycles to complete.
 *  
 *  @param[in] channel  0 to 7 corresponds to ADC0 to ADC7
 *  @return u32_t       Accumulated ADC value
 */
extern u32_t adc_get_samples(const u8_t channel, u8_t nr_of_samples);

/* _____MACROS_______________________________________________________________ */
/**
 *  @}
 */
#endif
