#ifndef __AVR_PIO_H__
#define __AVR_PIO_H__
/* =============================================================================

    Copyright (c) 2008 Pieter Conradie <pieterconradie@users.berlios.de>
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
    
    Title:          Atmel AVR Peripheral IO macros
    Author(s):      Pieter.Conradie
    Creation Date:  2008/11/01
    Revision Info:  $Id$

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup AVR_PIO avr_pio.h : Atmel AVR Peripheral IO macros
 *
 *  Collection of macros that simplifies defining and using Peripheral I/O.
 *  
 *  Files: avr_pio.h
 *  
 *  @par Example:
 *  @code
 *  // LED is on PORT A, pin 3, configured as an output, initial value is high
 *  #define PIO_LED  PIO(A,3,OUT,1)
 * 
 *  // Macro to enable LED (set PORTA, pin 3 to zero)
 *  #define LED_ON() BIT_SET_LO(PORT(PIO_LED),BIT(PIO_LED))
 * 
 *  // Macro to disable LED (set PORTA, pin 3 to zero)
 *  #define LED_OFF() BIT_SET_HI(PORT(PIO_LED),BIT(PIO_LED))
 *  
 *  // Configure pin
 *  DDR(PIO_LED) &= ~BIT_MASK(PIO_LED);
 *  DDR(PIO_LED) |=  DIR_MASK(PIO_LED);
 * 
 *  // Set initial value
 *  PORT(PIO_LED) &= ~BIT_MASK(PIO_LED);
 *  PORT(PIO_LED) |=  INIT_VAL_MASK(PIO_LED);
 * 
 *  // Enable LED
 *  LED_ON();
 *  @endcode
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */
/** 
 * Macro to encapsulate PIO definition
 *
 * @param port      A,B,C,...
 * @param bit       0,1,2,3,...
 * @param direction IN,OUT
 * @param init_val  OUTPUT: 0,1; INPUT: HI_Z,PULL_UP
 */
#define _PIO(port,bit,direction,init_val)  port, bit, direction, init_val
#define PIO(port,bit,direction,init_val)  _PIO(port,bit,direction,init_val)

// Macro to extract PORTx from PIO(...)
#define _PORT(port,bit,direction,init_val) CONCAT(PORT, port)
#define PORT(pio) _PORT(pio)

// Macro to extract DDRx from PIO(...)
#define _DDR(port,bit,direction,init_val) CONCAT(DDR, port)
#define DDR(pio) _DDR(pio)

/// Macro to extract PINx from PIO(...)
#define _PIN(port,bit,direction,init_val) CONCAT(PIN, port)
#define PIN(pio) _PIN(pio)

/// Macro to extract bit from PIO(...)
#define _BIT(port,bit,direction,init_val) bit
#define BIT(pio) _BIT(pio)

/// Macro to extract bit mask (1<<bit) from PIO(...)
#define _BIT_MASK(port,bit,direction,init_val) (1<<bit)
#define BIT_MASK(pio) _BIT_MASK(pio)

/// Macro to extract direction from PIO(...)
#define _DIR(port,bit,direction,init_val) direction
#define DIR(pio) _DIR(pio)

/// Macro to extract direction mask (direction<<bit) from PIO(...)
#define _DIR_MASK(port,bit,direction,init_val) (direction<<bit)
#define DIR_MASK(pio) _DIR_MASK(pio)

/// Macro to initial value from PIO(...)
#define _INIT_VAL(port,bit,direction,init_val) init_val
#define INIT_VAL(pio) _INIT_VAL(pio)

/// Macro to initial value mask from PIO(...)
#define _INIT_VAL_MASK(port,bit,direction,init_val) (init_val<<bit)
#define INIT_VAL_MASK(pio) _INIT_VAL_MASK(pio)

/// @name PIO parameter definitions
//@{
#define IN      0
#define OUT     1
#define HI_Z    0
#define PULL_UP 1
//@}

/**
 *  @}
 */
#endif
