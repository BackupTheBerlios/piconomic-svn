#ifndef __COMMON_H__
#define __COMMON_H__
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
    
    Title:          Common definitions
    Author(s):      Pieter Conradie
    Creation Date:  2008-02-05
    Revision Info:  $Id$

============================================================================= */
/** 
 *  @ingroup GENERAL
 *  @defgroup COMMON common.h : Common definitions
 *
 *  Definition of standard types, boolean, scope and utility
 *  macros.
 *
 *  File: common.h
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdint.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
// Include board specific definitions, e.g. processor frequency
#include "board.h"

/* _____DEFINITIONS _________________________________________________________ */
#ifndef NULL
/// NULL pointer
#define NULL ((void*)0)
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// @name Standard types
//@{
typedef uint8_t  u8_t;     ///< unsigned 8-bit value (0 to 255)
typedef int8_t   s8_t;     ///< signed 8-bit value (-128 to +127)
typedef uint16_t u16_t;    ///< unsigned 16-bit value (0 to 65535)
typedef int16_t  s16_t;    ///< signed 16-bit value (-32768 to 32767)
typedef uint32_t u32_t;    ///< unsigned 32-bit value (0 to 4294967296)
typedef int32_t  s32_t;    ///< signed 32-bit value (-2147483648 to +2147483647)
//@}

/// @name Boolean type
//@{
typedef enum
{
    FALSE = 0,
    TRUE  = !FALSE,
} bool_t;
//@}

/* _____MACROS_______________________________________________________________ */
/**
 * @name Type size and sign macros
 * 
 * Source: <a href="http://www.fefe.de/intof.html">"Catching Integer Overflows in C"</a>
 */
//@{
///@cond
#define __HALF_MAX_SIGNED(type) ((type)1 << (sizeof(type)*8-2))
#define __MAX_SIGNED(type) (__HALF_MAX_SIGNED(type) - 1 + __HALF_MAX_SIGNED(type))
#define __MIN_SIGNED(type) (-1 - __MAX_SIGNED(type))
///@endcond

/// Test if a type is signed or unsigned
#define TYPE_IS_SIGNED(type)((type)-1 < 1)

/**
 *  Return the minimum of a type.
 * 
 *  For an unsigned type it is zero.
 *  For a signed type this is the largest negative value
 */
#define MIN_OF_TYPE(type) (TYPE_IS_SIGNED(type)?__MIN_SIGNED(type):(type)0)

///  Return the maximum of a type.
#define MAX_OF_TYPE(type) ((type)~MIN_OF_TYPE(type))
//@}

/// @name Concatenation macros
//@{
///@cond
#define _CONCAT(x, y) x ## y
///@endcond
/** 
 *  Recursive token concatenation macro.
 * 
 * @par Example:
 * @code
 * #define XY       123456
 * #define TOKEN1   X
 * #define TOKEN2   Y
 * #define TOKEN1_2 CONCAT(X,Y)
 * @endcode
 * 
 * CONCAT(X,Y) is expanded by the C preprocessor to the macro XY, which is
 * expanded to 123456.
 * 
 */
#define CONCAT(x, y) _CONCAT(x, y)
//@}

/// @name Bit manipulation macros useful to manipulate Port IO pins
//@{
/// Macro to set a bit (1)
#define BIT_SET_HI(var,bit)           {var |=(1<<bit);}

/// Macro to clear a bit (0)
#define BIT_SET_LO(var,bit)           {var &= ~(1<<bit);}

/// Macro to toggle a bit
#define BIT_TOGGLE(var,bit)           {if(var&(1<<bit)) {var &= ~(1<<bit);} else {var |=(1<<bit);}}

/// Macro to test if a bit is set (1?)
#define BIT_IS_HI(var,bit)            ((var&(1<<bit)) != 0)

/// Macro to test if a bit is cleared (0?)
#define BIT_IS_LO(var,bit)            ((var&(1<<bit)) == 0)

/// Macro to wait until a bit is set
#define LOOP_UNTIL_BIT_IS_HI(var,bit) while(BIT_IS_LO(var,bit)) {;}

/// Macro to wait until a bit is cleared
#define LOOP_UNTIL_BIT_IS_LO(var,bit) while(BIT_IS_HI(var,bit)) {;}
//@}

///  @name Byte macros
//@{
/// Macro to extract the high 8 bits of a 16-bit value (Most Significant Byte)
#define U16_HI8(data) ((u8_t)((data>>8)&0xff))

/// Macro to extract the low 8 bits of a 16-bit value (Least Significant Byte)
#define U16_LO8(data) ((u8_t)(data&0xff))
//@}

/// @name General utility macros
//@{
/// Macro to calculate division with rounding to nearest integer value
#define DIV_ROUND(dividend,divisor)     (((dividend+((divisor)>>1))/(divisor)))

/// Macro to calculate division with rounding to nearest integer value
#define DIV_ROUNDUP(dividend,divisor)   (((dividend+((divisor)-1))/(divisor)))

/// Macro to check if a value is within bounds (min <= value <= max ?)
#define BOUND(value,min,max)            (((value)>=(min))&&((value)<=(max)))

/// Macro to calculate the length of an array
#define ARRAY_LENGTH(array)             (sizeof(array)/sizeof((array)[0]))
//@}

/**
 *  @}
 */
#endif
