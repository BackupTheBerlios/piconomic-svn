#ifndef __ARM_H__
#define __ARM_H__
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
    
    Title:          ARM helper functions
    Author(s):      Pieter Conradie
    Creation Date:  2008/06/30
    Revision Info:  $Id: arm.h 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/** 
 *  @ingroup ARM
 *  @defgroup ARM_HELPER arm.h : ARM helper functions
 *
 *  Defines various ARM helper functions to enable/disable IRQ and FIQ, etc.
 *  
 *  Files: arm.h & arm.c
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
#include "arm_defs.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 * Enable IRQ exception.
 * 
 * @return u32_t    The old CPSR value; used to remember the status of the I and F bit
 */
extern u32_t arm_enable_irq(void);

/** 
 * Disable IRQ exception.
 * 
 * @return u32_t    The old CPSR value; used to remember the status of the I and F bit
 */
extern u32_t arm_disable_irq(void);

/** 
 * Enable FIQ exception.
 * 
 * @return u32_t    The old CPSR value; used to remember the status of the I and F bit
 */
extern u32_t arm_enable_fiq(void);

/** 
 * Disable FIQ exception.
 * 
 * @return u32_t    The old CPSR value; used to remember the status of the I and F bit
 */
extern u32_t arm_disable_fiq(void);

/** 
 * Enable IRQ and FIQ exception.
 * 
 * @return u32_t    The old CPSR value; used to remember the status of the I and F bit
 */
extern u32_t arm_enable_irq_fiq(void);

/** 
 * Disable IRQ and FIQ exception.
 * 
 * @return u32_t    The old CPSR value; used to remember the status of the I and F bit
 */
extern u32_t arm_disable_irq_fiq(void);

/** 
 * Restore IRQ and FIQ exception.
 * 
 * @param cpsr      The old CPSR value; e.g. returned by arm_disable_irq()
 */
extern void arm_restore_irq_fiq(u32_t cpsr);

/* _____MACROS_______________________________________________________________ */
/**
 * Convenience macro for assembler instruction NOP.
 */
#define ARM_NOP() \
    __asm__ __volatile__("nop \n\t" : : );

/**
 * Convenience macro to place a funtion in .text.sram section.
 */
#define ARM_SECTION_TEXT_SRAM  __attribute__ ((section (".text.sram")))

/**
 *  @}
 */
#endif
