/* Copyright (c) 2002, Marek Michalkiewicz <marekm@amelek.gda.pl>
   Copyright (c) 2007, Eric B. Weddington
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
   POSSIBILITY OF SUCH DAMAGE. */

/* $Id: gcrt1.S,v 1.12.2.1 2008/03/24 17:11:06 arcanum Exp $ */

#if (__GNUC__ < 3) || (__GNUC__ == 3 && __GNUC_MINOR__ < 3)
#error "GCC version >= 3.3 required"
#endif

#include "macros.inc"

	.section .init0,"ax",@progbits
	.weak	__init
;	.func	__init
__init:

#ifndef __AVR_ASM_ONLY__
	.weak	__stack

	/* By default, malloc() uses the current value of the stack pointer
	   minus __malloc_margin as the highest available address.

	   In some applications with external SRAM, the stack can be below
	   the data section (in the internal SRAM - faster), and __heap_end
	   should be set to the highest address available for malloc().  */
	.weak	__heap_end
	.set	__heap_end, 0

	.section .init2,"ax",@progbits
	clr	__zero_reg__
	out	AVR_STATUS_ADDR, __zero_reg__
	ldi	r28,lo8(__stack)
#ifdef _HAVE_AVR_STACK_POINTER_HI
	ldi	r29,hi8(__stack)
	out	AVR_STACK_POINTER_HI_ADDR, r29
#endif	/* _HAVE_AVR_STACK_POINTER_HI */
	out	AVR_STACK_POINTER_LO_ADDR, r28

#ifdef __AVR_3_BYTE_PC__
	ldi	r16, hh8(pm(__vectors))
	out	_SFR_IO_ADDR(EIND), r16
#endif	/* __AVR_3_BYTE_PC__ */

#if BIG_CODE
	/* Only for >64K devices with RAMPZ, replaces the default code
	   provided by libgcc.S which is only linked in if necessary.  */

	.section .init4,"ax",@progbits
	.global __do_copy_data
__do_copy_data:
	ldi	r17, hi8(__data_end)
	ldi	r26, lo8(__data_start)
	ldi	r27, hi8(__data_start)
	ldi	r30, lo8(__data_load_start)
	ldi	r31, hi8(__data_load_start)

	/* On the enhanced core, "elpm" with post-increment updates RAMPZ
	   automatically.  Otherwise we have to handle it ourselves.  */

#ifdef __AVR_ENHANCED__
	ldi	r16, hh8(__data_load_start)
#else
	ldi	r16, hh8(__data_load_start - 0x10000)
.L__do_copy_data_carry:
	inc	r16
#endif
	out	AVR_RAMPZ_ADDR, r16
	rjmp	.L__do_copy_data_start
.L__do_copy_data_loop:
#ifdef __AVR_ENHANCED__
	elpm	r0, Z+
#else
	elpm
#endif
	st	X+, r0
#ifndef __AVR_ENHANCED__
	adiw	r30, 1
	brcs	.L__do_copy_data_carry
#endif
.L__do_copy_data_start:
	cpi	r26, lo8(__data_end)
	cpc	r27, r17
	brne	.L__do_copy_data_loop
#endif /* BIG_CODE */

	.set	__stack, RAMEND
#endif /* !__AVR_ASM_ONLY__ */

	.section .init9,"ax",@progbits

	rjmp	main

;	.endfunc

