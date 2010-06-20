/* =============================================================================

    Copyright (c) 2009 Pieter Conradie [www.piconomic.co.za]
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
    
    Title:          Simplified Linker Script example for 03_C_board_init
    Author(s):      Pieter Conradie
    Creation Date:  2009/02/20
    Revision Info:  $Id$

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
    FLASH (rx)   : ORIGIN = 0x00100000, LENGTH = 256k
    SRAM  (w!rx) : ORIGIN = 0x00200000, LENGTH = 64k
}

/*
 * Define stack size
 */
STACK_SIZE = 1024;


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
        KEEP (*(.vectors))
        . = ALIGN(4);
  } AT>FLASH

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
        *(.text)
        . = ALIGN(4);
  } > FLASH  

  /*
   * STACK section holds the stack for each processor mode.
   */
  .stack :
  {
        . += STACK_SIZE;    /* Allocate space for the stack */        
        .  = ALIGN(4);        
        _stack = . ;
  } > SRAM

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
