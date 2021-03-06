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
    
    Title:          AT91 Nested / Non Nested Interrupt Handler
    Author(s):      Pieter Conradie
    Creation Date:  2008/06/30
    Revision Info:  $Id: arm_irq.S 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/* 
 * References:
 *
 * [1] "Writing interrupt handlers", Technical Support FAQs, ARM
 *      http://www.arm.com/support/faqdev/1456.html
 *
 * [2] "What happens if an interrupt occurs as it is being disabled?", ARM
 *     http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/3682.html
 *
 * [3] "What happens if an interrupt occurs and the interrupt handler does not remove the interrupt?", ARM
 *     http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/3682.html
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "arm_defs.h"
#include "board.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____SECTION______________________________________________________________ */

.section .text.sram, "ax" /* Declare that the following code goes into the SRAM segment */
.code 32                  /* Select ARM instruction set */

.global arm_handler_irq
arm_handler_irq:
        /* 
         * Adjust LR_IRQ to correct return adres (compensate for pipelining) 
         * and save on the IRQ stack.
         */
        sub     lr, lr, #4
        stmfd   sp!, {lr}

        /* Make a copy of SPSR_IRQ (R14 = LR) */
        mrs     r14, SPSR

        /*
         * See if IRQ happened just as it was being disabled. Refer to [2]
         *
         * If this is the case return immediately. The interrupt will remain
         * pending since it has not been acknowledged. It will be reissued 
         * when interrupts are next enabled.
         */
        tst     r14, #ARM_I_BIT
        ldmnefd sp!, {pc}^

        /* Save R12 and R14(SPSR_IRQ) on the IRQ stack */
        stmfd   sp!, {r12, r14}

        /* 
         * Read the Interrupt Vector Registor (IVR) to determine which 
         * C handler to call.
         */
        ldr     r14, =AT91C_BASE_AIC
        ldr     r12, [r14, #AIC_IVR]

#ifdef DEBUG
        /* Write to the IVR to support Protect Mode */
        str     r12, [r14, #AIC_IVR]
#endif

#ifdef ARM_NO_NESTED_IRQ
        /* Change to SYSTEM mode (FIQ enabled) */
        msr     CPSR_c, #ARM_MODE_SYS | ARM_I_BIT
#else
        /* Change to SYSTEM mode (IRQ and FIQ enabled) */
        msr     CPSR_c, #ARM_MODE_SYS
#endif

        /*
         * Call C Handler in SYSTEM mode.
         *
         * All AAPCS registers used are saved to the SYSTEM stack, except
         * for R12 which is already saved to the IRQ stack. The handler must
         * save and restore other registers (if used).
         *
         * ARM-Thumb interworking is supported with "bx" instruction.
         *
         */        
        stmfd   sp!, {r0-r3, lr}
        mov     lr, pc
        bx      r12

        /* Restore AAPCS registers (except R12) from SYSTEM stack */            
        ldmfd   sp!, {r0-r3, lr}

        /* Return to IRQ mode with IRQ disabled (FIQ enabled) */
        msr     CPSR_c, #ARM_MODE_IRQ | ARM_I_BIT

        /* Acknowledge interrupt by writing to AIC_EOICR */
        ldr     r14, =AT91C_BASE_AIC
        str     r14, [r14, #AIC_EOICR]

        /* Restore R12 and R14 from IRQ stack */
        ldmfd   sp!, {r12, r14}

        /* Restore SPSR_IRQ */
        msr     SPSR_cxsf, r14

        /* Return and change mode (CPSR = SPSR_IRQ) */
        ldmfd   sp!, {pc}^

/* Literal pool (constants) */
.ltorg

.end

/* _____LOG__________________________________________________________________ */
/*

 2008/06/30 : Pieter.Conradie
 - Created
   
*/
