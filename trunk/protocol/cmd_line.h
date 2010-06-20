#ifndef __CMD_LINE_H__
#define __CMD_LINE_H__
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
    
    Title:          Command line parser
    Author(s):      Pieter Conradie
    Creation Date:  2008/08/01
    Revision Info:  $Id$

============================================================================= */

/** 
 *  @ingroup PROTOCOL
 *  @defgroup CMD_LINE cmd_line.h : Command line parser
 *
 *  Implements a command line parser using the ANSI/VT100 terminal protocol.
 *  
 *  Files: cmd_line.h & cmd_line.c
 *  
 *  @see
 *  - http://en.wikipedia.org/wiki/ANSI_escape_code
 *  - http://www.termsys.demon.co.uk/vtansi.htm
 *  
 *  Example:
 *  @include test/cmd_line_test.c
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/*
 * Definition for a pointer to a function that will be called to 
 * handle a command
 */
typedef const char* (*cmd_line_handler_t)(int argc, char* argv[]);

/*
 * Definition for a pointer to a function that will be called to 
 * send a character
 */
typedef void (*cmd_line_put_char_t)(char data);

/*
 * Definition of a command handler structure
 */
typedef struct cmd_line_s
{
    const char*        name;
    cmd_line_handler_t handler;
    const char*        help;
    struct cmd_line_s  *next_cmd;
    struct cmd_line_s  *parent_cmd;
    struct cmd_line_s  *child_cmd;
} cmd_line_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 * Initialise command line module
 * 
 * @param put_char  Function to call to send a character
 */
extern void cmd_line_init(cmd_line_put_char_t put_char);

/** 
 * Add a command to the list of commands.
 * 
 * @param cmd       Pointer to structure that contains the data for the new command
 * @param name      Name string of the command
 * @param handler   Handler that will be called when the command is invoked
 * @param help      Help string that is displayed for the command
 */
extern void cmd_line_add(cmd_line_t         *cmd,
                         const char         *name,
                         cmd_line_handler_t handler,
                         const char         *help);

/** 
 * Add a child command to the list of commands.
 * 
 * @param parent_cmd    Pointer to the parent command
 * @param cmd           Pointer to structure that contains the data for the new command
 * @param name          Name string of the command
 * @param handler       Handler that will be called when the command is invoked
 * @param help          Help string that is displayed for the command
 */
extern void cmd_line_add_child(cmd_line_t         *parent_cmd,
                               cmd_line_t         *cmd,
                               const char*        name,
                               cmd_line_handler_t handler,
                               const char         *help);

/** 
 * Function called to handle a received character.
 * 
 * This function drives the command line parser. All actions are taken in 
 * response to a received character.
 * 
 * @param rx_char   The received character.
 */
extern void cmd_line_process(char rx_char);

/** 
 * Helper function to convert a string to a number of type "long".
 * 
 * This function is useful to parse command parameters. The string is converted 
 * into a number and it is tested if the number is within the specified 
 * bounds (min <= val <= max).
 * 
 * @param str   String to convert
 * @param val   Pointer to location here number must be stored
 * @param min   Minimum bound for the number
 * @param max   Maximum bound for the number
 * 
 * @retval TRUE String has been succesfully converted into a number and lies between the specified bounds
 */
extern bool_t cmd_line_strtol(const char* str, long* val, long min, long max);

/* _____MACROS_______________________________________________________________ */

/**
 * @}
 */
#endif
