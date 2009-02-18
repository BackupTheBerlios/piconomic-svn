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
    
    Title:          ARM C Run Time Initialisation
    Author(s):      Pieter Conradie
    Creation Date:  2008/06/30
    Revision Info:  $Id: arm_crt0.S 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/*
 * References:
 *
 * [1] "Using gas, the GNU assembler (GNU Binutils 2.18 documentation)" 
 *     http://sourceware.org/binutils/docs-2.18/as/index.html
 *
 * [2] "ARM Architecture Reference Manual ARM DDI 0100E", David Seal
 *     http://www.arm.com/documentation/books.html
 *     http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0100e/index.html
 *
 * [3] "ARM System Developer's Guide", Sloss, Symes & Wright
 *     http://www.arm.com/documentation/books.html
 *
 * [4] "Procedure Call Standard for the ARM Architecture ARM IHI 0042B", ARM
 *     infocenter.arm.com/help/topic/com.arm.doc.ihi0042a/IHI0042A_aapcs.pdf
 *
 * [5] "Debugging with GDB, The GNU Source-Level Debugger" 
 *     http://sourceware.org/gdb/current/onlinedocs/gdb_toc.html
 *
 * [6] "Using the GNU Compiler Collection (GCC)"
 *     http://gcc.gnu.org/onlinedocs/gcc-4.3.0/gcc/ 
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "arm_defs.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____SECTION______________________________________________________________ */

.section .vectors, "ax" /* Declare that the following code goes into the .vectors segment */
.code 32                /* Select ARM instruction set */

.global _start          /* Specify global symbol */
_start:

/* 
 * Exception handling vector table
 * See [2], Table 2-3 "Exception processing modes", page A2-13
 */
#ifdef ARM_DEBUG_WAIT
vect_rst:  ldr pc, =vect_rst            /* 0x00 : Reset */
#else
vect_rst:  ldr pc, =arm_crt0_rst        /* 0x00 : Reset */
#endif

vect_und:  ldr pc, =arm_handler_und     /* 0x04 : Undefined Instruction */

vect_swi:  ldr pc, =arm_handler_swi     /* 0x08 : Software Interrupt (SWI) */

vect_pref: ldr pc, =arm_handler_pref    /* 0x0C : Prefetch Abort (instruction fetch memory abort) */

vect_data: ldr pc, =arm_handler_data    /* 0x10 : Data Abort (data access memory abort) */

vect_rsvd: nop                          /* 0x14 : Reserved */

#ifdef ARM_AUTO_VECTORING_IRQ
vect_irq:  ldr pc, [pc,#-0xF20]         /* 0x18 : IRQ (interrupt) */
#else
vect_irq:  ldr pc, =arm_handler_irq     /* 0x18 : IRQ (interrupt) */
#endif

vect_fiq:  ldr pc, =arm_handler_fiq     /* 0x1C : FIQ (fast interrupt) */

        /* Literal pool (constants) */
        .ltorg

/* _____SECTION______________________________________________________________ */

.section .init, "ax" /* Declare that the following code goes into the .init segment */
.code 32             /* Specify ARM instruction set */

arm_crt0_rst:
#ifdef str912fw44
        /* Wait for oscillator to stabilise - required for STR91x */
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
#endif

        /* Initialise stack pointer temporarily to the top of the stack */
        ldr     sp, =_stack

#ifdef DEBUG
        /*
         * Set stack limit to the bottom of the stack if stack checking is enabled.
         * See [6], §3.18 "Options for Code Generation Conventions", gcc switch -fstack-check
         */
        ldr     sl, =__stack_start

        /*
         * Initialise frame pointer to zero to support stack frames for debugger
         * See [5], §6.1 "Stack frames"
         *
         * R7 is used as the Thumb frame pointer.
         */
        mov     fp, #0
        mov	    r7, #0
#endif

#ifndef ARM_NO_REMAP
        /* Remapping support: Copy .vectors segment to .vectors.sram */
        ldr     r0, =__vectors_load
        ldr     r1, =__vectors_start
        ldr     r2, =__vectors_end
1:
        cmp     r1, r2
        ldrlo   r3, [r0], #4
        strlo   r3, [r1], #4
        blo     1b

        /* Copy executable code to .text.sram segment */
        ldr     r0, =__text_sram_load
        ldr     r1, =__text_sram_start
        ldr     r2, =__text_sram_end
1:
        cmp     r1, r2
        ldrlo   r3, [r0], #4
        strlo   r3, [r1], #4
        blo     1b
#endif

        /*
         * Initialise low level hardware by calling board_lowlevel_init().
         * 
         * ARM-Thumb Procedure calling standard (ATPCS) is followed for 
         * ARM/Thumb interworking code, to support case where board_lowlevel_init() 
         * is compiled in thumb mode WITH interworking.
         *
         * The return address (arm_crt0_init) is explicitely hard-coded ("mov lr,pc" not used)
         * to keep code position independant.
         */
        ldr     r0, =board_lowlevel_init
        ldr     lr, =arm_crt0_init
        bx      r0

arm_crt0_init:
        /* Copy initialisation values to .data segment */
        ldr     r0, =__data_load
        ldr     r1, =__data_start
        ldr     r2, =_edata
1:
        cmp     r1, r2
        ldrlo   r3, [r0], #4
        strlo   r3, [r1], #4
        blo     1b

        /* Clear the .bss segment */
        ldr     r0, =__bss_start__
        ldr     r1, =__bss_end__
        mov     r2, #0
1:
        cmp     r0, r1
        strlo   r2, [r0], #4
        blo     1b

#ifdef DEBUG
        /* Fill the .stack segment with magic values to debug stack overflows */
        ldr     r0, =__stack_start
        ldr     r1, =_stack
        ldr     r2, =0xCCCCCCCC
1:
        cmp     r0, r1
        strlo   r2, [r0], #4
        blo     1b
#endif

        /* Change modes (with IRQ and FIQ disabled) to setup stacks */
        msr     CPSR_c, #ARM_MODE_FIQ | ARM_I_BIT | ARM_F_BIT
        ldr     sp, =__stack_fiq_top

        msr     CPSR_c, #ARM_MODE_IRQ | ARM_I_BIT | ARM_F_BIT
        ldr     sp, =__stack_irq_top

        msr     CPSR_c, #ARM_MODE_UND | ARM_I_BIT | ARM_F_BIT
        ldr     sp, =__stack_und_top

        msr     CPSR_c, #ARM_MODE_ABT | ARM_I_BIT | ARM_F_BIT
        ldr     sp, =__stack_abt_top

        msr     CPSR_c, #ARM_MODE_SVC | ARM_I_BIT | ARM_F_BIT
        ldr     sp, =__stack_svc_top

        msr     CPSR_c, #ARM_MODE_SYS | ARM_I_BIT | ARM_F_BIT
        ldr     sp, =__stack_sys_usr_top

        /* End in SYSTEM mode with interrupts enabled */
        msr     CPSR_c, #ARM_MODE_SYS

        /*
         * Call main() using ATPCS
        */
        ldr     r0, =main
        ldr     lr, =_fini
        bx      r0

        /* Literal pool (constants) */
        .ltorg

/* _____SECTION______________________________________________________________ */

.section .fini, "ax"  /* Declare that the following code goes into the .fini segment */
.code 32              /* Select ARM instruction set */

.global _fini         /* Specify global symbol */
_fini:
        /* Loop forever */
        b       _fini

.end

/* _____LOG__________________________________________________________________ */
/*

 2008/06/30 : Pieter.Conradie
 - Created
   
*/
