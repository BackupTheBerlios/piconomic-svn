#ifndef __ARM_CP15_H__
#define __ARM_CP15_H__
/* =============================================================================

    Copyright (c) 2008 Pieter Conradie [www.piconomic.co.za]
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
    
    Title:          ARM CP15 Register access
    Author(s):      Pieter.Conradie
    Creation Date:  2009/03/30
    Revision Info:  $Id$

============================================================================= */

/** 
 *  @ingroup ARM
 * @defgroup ARM_CP15 arm_cp15.h : ARM CP15 Register access
 *
 *  Defines ARM helper functions to access CP15.
 *  
 *  Files: arm_cp15.h & arm_cp15.c
 *  
 *  References:
 *
 *  [1] "ARM Architecture Reference Manual ARM DDI 0100E", David Seal <br>
 *      http://www.arm.com/documentation/books.html <br>
 *      http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0100e/index.html <br>
 *  
 *  [2] "avr-libc Reference Manual; §9.6 Inline Assembler Cookbook" <br>
 *      http://www.nongnu.org/avr-libc/user-manual/inline_asm.html <br>
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
/**
 * Reads CP15
 * 
 * @return u32_t    The value of CP15
 */
extern u32_t arm_get_cp15(void);

/**
 * Writes a value to CP15
 * 
 * @param value     The value to write to CP15
 */
extern void arm_set_cp15(u32_t value);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
