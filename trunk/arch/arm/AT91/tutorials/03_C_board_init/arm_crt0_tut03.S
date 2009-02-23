/* =============================================================================

    Copyright (c) 2009 Pieter Conradie <pieterconradie@users.berlios.de>
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
    
    Title:          ARM C Run Time Initialisation for example 03_C_board_init
    Author(s):      Pieter Conradie
    Creation Date:  2009/02/20
    Revision Info:  $Id$

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
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */

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
vect_rst:  ldr pc, =arm_crt0_rst        /* 0x00 : Reset */

vect_und:  ldr pc, =arm_handler_und     /* 0x04 : Undefined Instruction */

vect_swi:  ldr pc, =arm_handler_swi     /* 0x08 : Software Interrupt (SWI) */

vect_pref: ldr pc, =arm_handler_pref    /* 0x0C : Prefetch Abort (instruction fetch memory abort) */

vect_data: ldr pc, =arm_handler_data    /* 0x10 : Data Abort (data access memory abort) */

vect_rsvd: nop                          /* 0x14 : Reserved */

vect_irq:  ldr pc, =arm_handler_irq     /* 0x18 : IRQ (interrupt request) */

vect_fiq:  ldr pc, =arm_handler_fiq     /* 0x1C : FIQ (fast interrupt request) */

/* Unhandled exeption vectors */
arm_handler_und:
        b       arm_handler_und
arm_handler_swi:
        b       arm_handler_swi
arm_handler_pref:
        b       arm_handler_pref
arm_handler_data:
        b       arm_handler_data
arm_handler_irq:
        b       arm_handler_irq
arm_handler_fiq:
        b       arm_handler_fiq

/* Literal pool (constants) */
.ltorg

/* _____SECTION______________________________________________________________ */

.section .init, "ax" /* Declare that the following code goes into the .init segment */
.code 32             /* Specify ARM instruction set */

arm_crt0_rst:

        /* Initialise stack pointer temporarily to the top of the stack */
        ldr     sp, =_stack

        /* Initialise low level hardware by calling board_lowlevel_init() */
        ldr     r0, =board_lowlevel_init
        mov     lr, pc
        bx      r0

1:
        /* Loop forever */
        nop
        b       1b

/* Literal pool (constants) */
.ltorg

/* _____LOG__________________________________________________________________ */
/*

 2009/02/20 : Pieter.Conradie
 - Created
   
*/
