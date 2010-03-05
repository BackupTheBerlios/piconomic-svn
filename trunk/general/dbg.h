#ifndef __DBG_H__
#define __DBG_H__
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

    Title:          Debug module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id$

============================================================================= */
/** 
 *  @ingroup GENERAL
 *  @defgroup DBG dbg.h : Debug module
 *
 *  Outputs debug information via @ref PRINTF_MODULE
 *
 *  Files: dbg.h
 *
 *  An old school debugging practice is to add debug print statements in the source
 *  code to track program flow and check for coding mistakes during development. 
 *  These debug statements can be enabled for the debug build by defining
 *  #DBG to 1 and #DBG_LEVEL with the required output level globally
 *  (e.g. -DDBG=1 -DDBG_LEVEL=DBG_LEVEL_ALL in the Makefile) or on a per file
 *  basis.
 *  
 *  @par Example:
 *  @include "dbg_test.c"
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
#include "printf.h"

/* _____DEFINITIONS _________________________________________________________ */
/// Flag to disable (0) or enable (1) debug.
#ifndef DBG
#define DBG 0
#endif

/// @name Debug level bitmask definitions
//@{
/// None
#define DBG_LEVEL_NONE    0
/// Errors
#define DBG_LEVEL_ERR     (1<<0)
/// Warnings
#define DBG_LEVEL_WARN    (1<<1)
/// Progress
#define DBG_LEVEL_PROG    (1<<2)
/// All
#define DBG_LEVEL_ALL    (DBG_LEVEL_ERR|DBG_LEVEL_WARN|DBG_LEVEL_PROG)
//@}

#ifndef DBG_LEVEL
/**
 * Global debug output level.
 *
 * It is a bitmask that sets which debug info will be emmitted. E.g.
 * - #DBG_LEVEL = DBG_LEVEL_NONE : No debug output
 * - #DBG_LEVEL = DBG_LEVEL_NONE  : Report errors only
 * - #DBG_LEVEL = (DBG_LEVEL_NONE|DBG_LEVEL_WARN) : Report errors + warnings
 * - #DBG_LEVEL = (DBG_LEVEL_NONE|DBG_LEVEL_WARN|DBG_LEVEL_PROG) : Report errors + warnings + progress 
 */
#define DBG_LEVEL DBG_LEVEL_ERR
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____MACROS_______________________________________________________________ */
#if DBG
///@cond
// 1st part macro to convert a number to a string (GCC specific)
#define _DBG_STRINGIFY(number) #number
// 2nd part macro to convert a number to a string (GCC specific)
#define _DBG_NR_TO_STR(number) _DBG_STRINGIFY(number)
///@endcond

/**
 *  Macro that will output debug output if #DBG_LEVEL is defined as non zero.
 *
 *  @note The format of this macro is specific to the GCC preprocessor.
 * 
 *  @param[in] format Format string following by a variable list of arguments.
 *  
 */
#define DBG_TRACE(format, ...) \
            { \
                if(DBG_LEVEL != 0) \
                    PRINTF(format, ## __VA_ARGS__); \
            }

/**
 *  Macro that will output debug output if the specified level bit is set in #DBG_LEVEL.
 *
 *  The file name and line number is prepended to the format string to form one string.
 *  @note The format of this macro is specific to the GCC preprocessor.
 * 
 *  @param[in] level  The debug severity level (progress, warning or error)
 *  @param[in] format Format string following by a variable list of arguments.
 *  
 */
#define DBG_LOG(level, format, ...) \
            { \
                if(level & DBG_LEVEL) \
                    DBG_TRACE(__FILE__ " " _DBG_NR_TO_STR(__LINE__) " : " format, ## __VA_ARGS__); \
            }

/**
 *  Macro that will test an expression, and block indefinitely if FALSE.
 * 
 *  This macro will perform the test and if FALSE, will output the filename and line number
 *  with the test appended. The macro will then block indefinitely.
 *
 *  @param[in] expression Expression that evaluates to a boolean value (TRUE or FALSE)
 */
#define DBG_ASSERT(expression) \
            { \
                if((DBG_LEVEL != 0) && (!(expression))) \
                { \
                    DBG_TRACE(__FILE__ " " _DBG_NR_TO_STR(__LINE__) " : ASSERT " #expression); \
                    for(;;) {;} \
                } \
            }
#else
#define DBG_TRACE(format, ...)
#define DBG_LOG(level, format, ...)
#define DBG_ASSERT(expression)
#endif

/**
 * Shortcut macro to display an error message. 
 *  
 * @see DBG_LOG
 */
#define DBG_ERR(format, ...)  DBG_LOG(DBG_LEVEL_ERR,  format, ## __VA_ARGS__)

/**
 * Shortcut macro to display a warning message. 
 *  
 * @see DBG_LOG
 */
#define DBG_WARN(format, ...) DBG_LOG(DBG_LEVEL_WARN, format, ## __VA_ARGS__)

/**
 * Shortcut macro to display a progress message. 
 *  
 * @see DBG_LOG
 */
#define DBG_PROG(format, ...) DBG_LOG(DBG_LEVEL_PROG, format, ## __VA_ARGS__)

/**
 *  @}
 */
#endif
