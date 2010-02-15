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
    
    Title:          AT91SAM7S256 ROM Linker Script
    Author(s):      Pieter Conradie
    Creation Date:  2008/06/30
    Revision Info:  $Id: at91sam7s64-rom.lds 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/* 
 * Based on Newlib-1.16.0 Default linker script, for normal executables
 * (arm-elf\lib\ldscripts\armelf.x)
 *
 * References:
 *
 * [1] "GNU ld (GNU Binutils) 2.18 documentation"
 *     http://sourceware.org/binutils/docs-2.18/ld/index.html
 *
 * [2] "System V Application Binary Interface", SCO
 *     http://www.sco.com/developers/devspecs/gabi41.pdf
 *     § "Special Sections", page 4-18 to 4-20, gives short explanation of each 
 *     section (.text, .bss,...)
 *
 * Notes:
 *
 * Full output section description:
 *
 *      section [address] [(type)] :
 *      [AT(lma)] [ALIGN(section_align)] [SUBALIGN(subsection_align)]
 *      {
 *      output-section-command
 *      output-section-command
 *      ...
 *      } [>region] [AT>lma_region] [:phdr :phdr ...] [=fillexp]
 *
 * See [1], §3.6.1, "Output Section Description"
 *
 * VMA = virtual memory address; executing address of section
 * LMA = load memory address; address at which the section will be loaded
 * See [1], §3.1 "Basic Linker Script Concepts"
 *
 * PROVIDE = define a symbol only if it is referenced and is not defined by any object
 * See [1], §3.5.2 "PROVIDE"
 *
 */

/*
 * Set the BFD output format based on the desired endianness
 * (default, big, little).
 */
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm","elf32-littlearm")

/*
 * Specify the output machine architecture.
 */
OUTPUT_ARCH(arm)

/*
 * Specify the program entry point (first instruction to be executed)
 */
ENTRY(_start)

/*
 * Describe the location and size of blocks of memory in the target
 */
MEMORY
{
    FLASH (rx)   : ORIGIN = 0x00100000, LENGTH = 64k
    SRAM  (w!rx) : ORIGIN = 0x00200000, LENGTH = 16k
}

/*
 * Define stack sizes
 */
STACK_SIZE_SYS_USR = 1024;
STACK_SIZE_SVC     = 0;
STACK_SIZE_ABT     = 0;
STACK_SIZE_UND     = 0;
STACK_SIZE_IRQ     = 8*12; /* 8 level deep nested IRQ stack, 3 registers pushed on stack */
STACK_SIZE_FIQ     = 0;

/* 
 * Inform linker how to map input sections into output sections, and how to 
 * place the output sections in memory.
 */
SECTIONS
{
  /*
   * VECTORS section holds the ARM exception vectors.
   * These instructions are executed at 0x0 (VMA), but loaded at the start of FLASH (LMA)
   */
  .vectors 0x00000000 :
  {
        __vectors_load  = LOADADDR(.vectors);
        KEEP (*(.vectors))
        . = ALIGN(4);
  } AT>FLASH

  /* Remapping support: Allocate space for VECTORS copied to SRAM */
  .vectors.sram :
  {
        __vectors_start = . ;
        . += SIZEOF(.vectors);
        __vectors_end = . ;
  } > SRAM

  /* 
   * Code in internal SRAM normally execute faster than in FLASH.
   * This is the prefered location for code that is frequently executed 
   * or needs to execute fast, e.g. IRQ and FIQ handler.
   * Code can be placed in this section by using the following C attribute:
   * __attribute__ ((section (".text.sram")))
   */
  .text.sram :
  {
        __text_sram_load = LOADADDR(.text.sram);
        __text_sram_start = . ;
        *(.text.sram .text.sram*)

        /* Prefered location of ARM-Thumb interworking veneers */
        *(.glue_7t)
        *(.glue_7)
        . = ALIGN(4);
        __text_sram_end = . ;
  } > SRAM AT>FLASH

  /*
   * INIT section holds executable instructions that contribute to the process
   * initialization code.
   */
  .init :
  {
        *(.init)
        . = ALIGN(4);
  } > FLASH

  /*
   * TEXT segment holds executable instructions
   */
  .text :
  {
        *(.text .stub .text.* .gnu.linkonce.t.*)

        /* ARM-Thumb interworking veneers */
        *(.glue_7t)
        *(.glue_7)
        . = ALIGN(4);
  } > FLASH

  /*
   * FINI section holds executable instructions that contribute to the process
   * termination code.
   */
  .fini :
  {
        *(.fini)
        . = ALIGN(4);
  } > FLASH  

  /*
   * RODATA and RODATA1 sections hold read-only data (constants).
   */
  .rodata :
  { 
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        *(.rodata1)
        . = ALIGN(4);
  } > FLASH

  /*
   * DATA section holds initialized data; initialised on startup.
   * Initialisation values is stored in FLASH (LMA) and copied to SRAM (VMA)
   */
  .data :
  {
        __data_load  = LOADADDR(.data);
        __data_start = . ;
        
        *(.data .data.* .gnu.linkonce.d.*)
        KEEP (*(.gnu.linkonce.d.*personality*))
        SORT(CONSTRUCTORS)
        *(.data1)
        . = ALIGN(4);
        
        _edata = .;
        PROVIDE (edata = .);
  } > SRAM AT>FLASH

  /*
   * BSS section holds uninitialized data ; set to zero on startup.
   */
  .bss (NOLOAD) :
  {
        __bss_start   = .;
        __bss_start__ = .;
        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(4);

        _bss_end__  = . ;
        __bss_end__ = . ;
  } > SRAM

  /*
   * STACK section holds the stack for each processor mode.
   */
  .stack :
  {
        __stack_start = . ;

        /* FIQ Stack */
        . += STACK_SIZE_FIQ;
        .  = ALIGN(4);
        __stack_fiq_top = . ;

        /* IRQ Stack */
        . += STACK_SIZE_IRQ;
        .  = ALIGN(4);
        __stack_irq_top = . ;

        /* UND Stack */
        . += STACK_SIZE_UND;
        .  = ALIGN(4);
        __stack_und_top = . ;

        /* ABT Stack */
        . += STACK_SIZE_ABT;
        .  = ALIGN(4);
        __stack_abt_top = . ;

        /* SVC Stack */
        . += STACK_SIZE_SVC;
        .  = ALIGN(4);
        __stack_svc_top = . ;

        /* SYS/USR Stack */
        . += STACK_SIZE_SYS_USR;
        .  = ALIGN(4);
        __stack_sys_usr_top = . ;
        
        _stack = . ;
  } > SRAM

  /* Mark the start of heap (if used) */
  __end__ = . ;
  _end    = . ;
  PROVIDE (end = . );

  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */

  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }

  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }

  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }

  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

  /* DWARF 3 */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }
    
  .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }
  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }

  /*
   * DISCARD section.
   * Discard input sections. See [1], §3.6.7 "Output Section Discarding"
   */
  /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) }
}
