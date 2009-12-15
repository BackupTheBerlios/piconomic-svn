#ifndef __PRINTF_H__
#define __PRINTF_H__
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
/** 
 *  @ingroup AVR
 *  @defgroup PRINTF_MODULE printf.h : PRINTF module
 *
 *  Initialises the stdout stream to output to UART0.
 *
 *  Files:
 *  - avr/printf.h
 *  - avr/printf.c
 *
 *  This component initialises the @b stdout stream to output over UART0. A macro
 *  #PRINTF is provided that will store the format string in program memory (as
 *  opposed to wasting data memory) and call @b printf_P with the variable number of
 *  arguments.
 *
 *  @note The handler for outgoing data will intercept all new line characters (@b \\n)
 *        and replace it with a carriage return, new line sequence (@b \\r\\n).
 *  
 *  @par Example:
 *  @include printf_test.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <avr/pgmspace.h>

/* _____PROJECT INCLUDES_____________________________________________________ */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise @b stdout stream to use UART0.
 * 
 *  printf_put_char() is provided as the handler for all outgoing data. A
 *  @b printf_stream structure is declared statically so that heap memory is 
 *  not used (malloc).
 */
extern void printf_init(void);

/* _____MACROS_______________________________________________________________ */
/**
 *  Variable argument macro that calls @b printf_P with format string stored in
 *  program memory.
 *
 *  @note This macro is specific to the GCC preprocessor.
 *
 *  @param[in] format   format string that will be stored in program memory
 *  @param[in] ...      optional, variable number of arguments
 */
#define PRINTF(format, ...) printf_P(PSTR(format), ## __VA_ARGS__)

/**
 *  @}
 */
#endif
