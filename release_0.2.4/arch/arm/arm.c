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
    
    Title:          ARM Specific definitions and code
    Author(s):      Pieter Conradie
    Creation Date:  2008/06/30
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "arm.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#ifdef __thumb__
#error "This C file must be compiled in ARM mode"
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
/* Default exception handlers.
 *
 * These handler are declared as "weak" so that they can be redefined.
 * If the code ends here, it means that a handler has not been defined
 * to handle the exception.
 *
 */
__attribute__ ((weak)) void arm_handler_und(void)
{
    for(;;)
    {
        ;
    }
}

__attribute__ ((weak)) void arm_handler_swi(void)
{
    for(;;)
    {
        ;
    }
}

__attribute__ ((weak)) void arm_handler_pref(void)
{
    for(;;)
    {
        ;
    }
}

__attribute__ ((weak)) void arm_handler_data(void)
{
    for(;;)
    {
        ;
    }
}

__attribute__ ((weak)) void arm_handler_irq(void)
{
    for(;;)
    {
        ;
    }
}

__attribute__ ((weak)) void arm_handler_fiq(void)
{
    for(;;)
    {
        ;
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
ARM_SECTION_TEXT_SRAM u32_t arm_enable_irq(void)
{
    u32_t cpsr;
    u32_t tmp;

    // Read, modify, write sequence to clear I bit
    __asm__ __volatile__("mrs %[cpsr], CPSR     \n\t"
                         "mov %[tmp], %[cpsr]   \n\t"
                         "bic %[tmp], %[mask]  \n\t"
                         "msr CPSR_c,%[tmp]    \n\t"
                         : [cpsr] "=&r" (cpsr),[tmp] "=&r" (tmp): [mask] "i" (ARM_I_BIT) );

    // Return original CPSR value, before it was modified
    return cpsr;
}

ARM_SECTION_TEXT_SRAM u32_t arm_disable_irq(void)
{
    u32_t cpsr;
    u32_t tmp;

    // Read, modify, write sequence to set I bit
    __asm__ __volatile__("mrs %[cpsr], CPSR     \n\t"
                         "mov %[tmp], %[cpsr]   \n\t"
                         "orr %[tmp], %[mask]  \n\t"
                         "msr CPSR_c,%[tmp]    \n\t"
                         : [cpsr] "=&r" (cpsr),[tmp] "=&r" (tmp): [mask] "i" (ARM_I_BIT) );

    // Return original CPSR value, before it was modified
    return cpsr;
}

ARM_SECTION_TEXT_SRAM u32_t arm_enable_fiq(void)
{
    u32_t cpsr;
    u32_t tmp;

    // Read, modify, write sequence to clear F bit
    __asm__ __volatile__("mrs %[cpsr], CPSR     \n\t"
                         "mov %[tmp], %[cpsr]   \n\t"
                         "bic %[tmp], %[mask]  \n\t"
                         "msr CPSR_c,%[tmp]    \n\t"
                         : [cpsr] "=&r" (cpsr),[tmp] "=&r" (tmp): [mask] "i" (ARM_F_BIT) );

    // Return original CPSR value, before it was modified
    return cpsr;
}

ARM_SECTION_TEXT_SRAM u32_t arm_disable_fiq(void)
{
    u32_t cpsr;
    u32_t tmp;

    // Read, modify, write sequence to set F bit
    __asm__ __volatile__("mrs %[cpsr], CPSR     \n\t"
                         "mov %[tmp], %[cpsr]   \n\t"
                         "orr %[tmp], %[mask]  \n\t"
                         "msr CPSR_c,%[tmp]    \n\t"
                         : [cpsr] "=&r" (cpsr),[tmp] "=&r" (tmp): [mask] "i" (ARM_F_BIT) );

    // Return original CPSR value, before it was modified
    return cpsr;
}

ARM_SECTION_TEXT_SRAM u32_t arm_enable_irq_fiq(void)
{
    u32_t cpsr;
    u32_t tmp;

    // Read, modify, write sequence to clear I and F bit
    __asm__ __volatile__("mrs %[cpsr], CPSR     \n\t"
                         "mov %[tmp], %[cpsr]   \n\t"
                         "bic %[tmp], %[mask]  \n\t"
                         "msr CPSR_c,%[tmp]    \n\t"
                         : [cpsr] "=&r" (cpsr),[tmp] "=&r" (tmp): [mask] "i" (ARM_I_BIT | ARM_F_BIT) );

    // Return original CPSR value, before it was modified
    return cpsr;
}

ARM_SECTION_TEXT_SRAM u32_t arm_disable_irq_fiq(void)
{
    u32_t cpsr;
    u32_t tmp;

    // Read, modify, write sequence to set I and F bit
    __asm__ __volatile__("mrs %[cpsr], CPSR     \n\t"
                         "mov %[tmp], %[cpsr]   \n\t"
                         "orr %[tmp], %[mask]  \n\t"
                         "msr CPSR_c,%[tmp]    \n\t"
                         : [cpsr] "=&r" (cpsr),[tmp] "=&r" (tmp): [mask] "i" (ARM_I_BIT | ARM_F_BIT) );

    // Return original CPSR value, before it was modified
    return cpsr;
}

ARM_SECTION_TEXT_SRAM void arm_restore_irq_fiq(u32_t cpsr)
{
    __asm__ __volatile__("msr CPSR_c,%[cpsr] \n\t" : : [cpsr] "r" (cpsr));
}


/* _____LOG__________________________________________________________________ */
/*

 2008/06/30 : Pieter.Conradie
 - Created
   
*/

