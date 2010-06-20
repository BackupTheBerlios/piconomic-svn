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
    
    Title:          STR91x FIQ Interrupt Handler
    Author(s):      Pieter Conradie
    Creation Date:  2008/09/22
    Revision Info:  $Id: arm_fiq.S 1 2008-12-02 07:02:22Z Pieter.Conradie $

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
 *
 * [4] "AN2593 - STR91x interrupt management", ST
 *     www.st.com/stonline/books/pdf/docs/13715.pdf
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "arm_defs.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____SECTION______________________________________________________________ */

.section .text.sram, "ax" /* Declare that the following code goes into the SRAM segment */
.code 32                  /* Select ARM instruction set */

.global handler_fiq
handler_fiq:
        /* 
         * Adjust LR_FIQ to correct return adres (compensate for pipelining) 
         * and save on the FIQ stack.
         */
        sub     lr, lr, #4

        /*
         * Call C Handler in FIQ mode. 
         *
         * All registers are saved to the FIQ stack (R8 to R14 already saved).
         *
         * ARM-Thumb interworking is supported with "bx R12" instruction.
         *
         */        
        ldr     r12,=FIQ_Handler
        stmfd   sp!, {r0-r7, lr}
        mov     lr, pc
        bx      r12

        /* Restore registers and return and change mode (CPSR = SPSR_FIQ) */
        ldmfd   sp!,{r0-r7,pc}^

/* Literal pool (constants) */
.ltorg

.end

/* _____LOG__________________________________________________________________ */
/*

 2008/09/22 : Pieter.Conradie
 - Created
   
*/
