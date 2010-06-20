/* =============================================================================

    Copyright (c) 2006 Pieter Conradie [www.piconomic.co.za]
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

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "cmd_line.h"
#include "vt100.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Maximum number of arguments after command
#define CMDL_ARGV_MAX         8

// Define the maximum length of a command line
#define CMDL_LINE_LENGTH_MAX  64

// Define the command line history size
#define CMDL_HISTORY_SIZE     128

// Help text column start
#define CMDL_HELP_TEXT_COLUMN 20

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Head of linked list of command structures
static cmd_line_t*  cmd_line_first_cmd;

/// List of pointers to strings (command and parameters)
static char* cmd_line_argv[CMDL_ARGV_MAX];

/// Buffer for command line
static u8_t cmd_line_buffer_index;
static char cmd_line_buffer[CMDL_LINE_LENGTH_MAX];

/// Buffer for history
static u8_t cmd_line_hist_index;
static char cmd_line_hist[CMDL_HISTORY_SIZE];

/// Function to call to output a character
static cmd_line_put_char_t cmd_line_put_char;

/// Help command structure
static cmd_line_t   cmd_line_help;

/// Command prompt string
static const char cmd_line_str_prompt[] = ">";

/// Command not found string
static const char cmd_line_str_cmd_not_found[] = "Error! Command not found\n\r";

/// Terminal delete instructions (to remove last character from display)
static const char cmd_line_str_del[] = {VT100_BS,' ',VT100_BS};

/// Parent-child tree string
static const char cmd_line_str_tree[] = {'+','-',' '};

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void cmd_line_send_new_line(void)
{
    (*cmd_line_put_char)('\n');
    (*cmd_line_put_char)('\r');
}

static void cmd_line_send_str(const char *data)
{
    if(data == NULL)
    {
        return;
    }

    while(*data != '\0')
    {
        (*cmd_line_put_char)(*data++);
    }
}

static void cmd_line_send_buffer(const char *data, u8_t number_of_bytes)
{
    while(number_of_bytes != 0)
    {
        (*cmd_line_put_char)(*data++);
        number_of_bytes--;
    }
}

static void cmd_line_save_hist(const char *cmd_str)
{
}

static void cmd_line_invoke(char *cmd_str)
{
    cmd_line_t *cmd;
    int        argc         = 0;
    char       *cmd_char    = cmd_str;
    const char *display_str;
    int        arg_index    = 0;

    // Mark first token string
    cmd_line_argv[argc++] = cmd_str;

    // Break command string up into null separated strings
    while(*cmd_char != '\0')
    {
        // See if whitespace is detected
        if(*cmd_char == ' ')
        {
            // Replace with terminating zero
            *cmd_char++ = '\0';

            // Eat whitespace
            while(*cmd_char == ' ')
            {
                cmd_char++;
            }

            // Mark next token string
            cmd_line_argv[argc++] = cmd_char;
        }
        else
        {
            // Next character
            cmd_char++;
        }
    }

    // See if last string was empty
    if(cmd_line_argv[argc-1][0] == '\0')
    {
        // Remove string from list
        argc--;
    }

    // Ignore empty command
    if(argc == 0)
    {
        return;
    }
    
    // Find command
    arg_index  = 0;
    cmd = cmd_line_first_cmd;
    while(cmd != NULL)
    {
        // See if string matches command
        if(strcmp(cmd_line_argv[arg_index],cmd->name) == 0)
        {
            // See if this is a parent command and there are more strings (for child command(s))
            if((cmd->child_cmd != NULL)&&((arg_index+1) < argc))
            {
                // Traverse to child list
                cmd = cmd->child_cmd;

                // Next argument in list
                arg_index++;
            }
            else
            {
                // Invoke command with extra parameters (command string(s) removed)
                if(cmd->handler)
                {
                    display_str = (*(cmd->handler))(argc-arg_index-1, &cmd_line_argv[arg_index+1]);
                    if(display_str != NULL)
                    {
                        // Display returned string from command handler
                        cmd_line_send_str(display_str);                        
                    }
                    cmd_line_send_new_line();
                }
                return;
            }
        }
        else
        {
            // Next item in list
            cmd = cmd->next_cmd;
        }
    }

    // Command not found in list
    cmd_line_send_str(cmd_line_str_cmd_not_found);
    return;
}

static void cmd_line_add_to_list(cmd_line_t** first_cmd, cmd_line_t* new_cmd)
{
    // Start at the first item in the linked list
    cmd_line_t** cmd = first_cmd;

    // Find last item in list's pointer (which will point to NULL)
    while(*cmd != NULL)
    {
        // Next item in list
        cmd = &((*cmd)->next_cmd);
    }

    // Add item to the end of the list
    *cmd = new_cmd;

    // Terminate item (mark item as the last item in the list)
    new_cmd->next_cmd = NULL;
}

static const char* cmd_line_help_handler(int argc, char* argv[])
{
    int i;
    int level       = 0;
    int indent      = 0;
    cmd_line_t* cmd = cmd_line_first_cmd;

    // Iterate through commands    
    while(cmd != NULL)
    {
        // Display hierarchy level
        indent = CMDL_HELP_TEXT_COLUMN - strlen(cmd->name);
        if(level != 0)
        {
            for(i=level; i>0; i--)
            {
                (*cmd_line_put_char)('|');
                indent--;
            }
            (*cmd_line_put_char)('-');
            indent--;
            (*cmd_line_put_char)(' ');
            indent--;
        }

        // Display command string
        cmd_line_send_str(cmd->name);

        // Indent help text
        for(i=indent; i>0; i--)
        {
            (*cmd_line_put_char)(' ');
        }

        // Display help text
        cmd_line_send_str(cmd->help);

        cmd_line_send_new_line();
        
        // See if this is a parent command
        if(cmd->child_cmd != NULL)
        {
            // Increase indent level
            level++;

            // Traverse child list
            cmd = cmd->child_cmd;
        }
        else
        {
            // See if the end of a child list has been reached
            if((cmd->next_cmd == NULL)&&(cmd->parent_cmd != NULL))
            {
                // Decrease indent level
                level--;
                // Go to next item after parent
                cmd = cmd->parent_cmd->next_cmd;
            }
            else
            {
                // Next item in list
                cmd = cmd->next_cmd;
            }
        }
    }

    return NULL;
}

static void cmd_line_disp_prompt(void)
{
    cmd_line_send_str(cmd_line_str_prompt);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void cmd_line_init(cmd_line_put_char_t put_char)
{
    int i;

    // No items in command list
    cmd_line_first_cmd = NULL;

    // Reset command line buffer
    cmd_line_buffer_index = 0;

    // Clear history buffer
    cmd_line_hist_index = 0;
    for(i=(CMDL_HISTORY_SIZE-1); i!=0; i--)
    {
        cmd_line_hist[i] = '\0';
    }

    // Remember function to call when a character must be outputted
    cmd_line_put_char = put_char;

    // Add help command
    cmd_line_add(&cmd_line_help,"help",&cmd_line_help_handler,"display list of commands");

    cmd_line_disp_prompt();
}

void cmd_line_add(cmd_line_t*         cmd,
                  const char*         name,
                  cmd_line_handler_t  handler,
                  const char*         help)
{
    // Populate structure
    cmd->name       = name;
    cmd->handler    = handler;
    cmd->help       = help;
    cmd->next_cmd   = NULL;
    cmd->parent_cmd = NULL;
    cmd->child_cmd  = NULL;

    // Add to linked list
    cmd_line_add_to_list(&cmd_line_first_cmd, cmd);
}

void cmd_line_add_child(cmd_line_t*        parent_cmd,
                        cmd_line_t*        cmd,
                        const char*        name,
                        cmd_line_handler_t handler,
                        const char*        help)
{
    // Populate structure
    cmd->name       = name;
    cmd->handler    = handler;
    cmd->help       = help;
    cmd->next_cmd   = NULL;
    cmd->parent_cmd = parent_cmd;
    cmd->child_cmd  = NULL;

    // Add to linked list of parent
    cmd_line_add_to_list(&(parent_cmd->child_cmd), cmd);
}

void cmd_line_process(char rx_char)
{
    // Preprocess received characters to detect VT100 escape codes
    rx_char = vt100_process_rx_char(rx_char);

    // See if ENTER has been pressed
    if(rx_char == VT100_CR)
    {
        // Terminate line
        cmd_line_send_new_line();

        // Mark end of command line
        cmd_line_buffer[cmd_line_buffer_index] = '\0';

        // Invoke command
        cmd_line_invoke(cmd_line_buffer);

        // Reset buffer
        cmd_line_buffer_index = 0;

        cmd_line_disp_prompt();
        return;
    }

    // See if BACK SPACE has been pressed
    if(rx_char == VT100_BS)
    {
        if(cmd_line_buffer_index>0)
        {
            // Remove last character from buffer
            cmd_line_buffer_index--;

            // Remove last character from terminal screen
            cmd_line_send_buffer(cmd_line_str_del,ARRAY_LENGTH(cmd_line_str_del));
        }
        return;
    }

    // Ignore invalid values
    if((rx_char < 0x20)||(rx_char > 0x7f))
    {
        return;
    }

    // Add character if buffer is not full
    if(cmd_line_buffer_index < (CMDL_LINE_LENGTH_MAX-1))
    {
        cmd_line_buffer[cmd_line_buffer_index++] = rx_char;
        // Echo character
        (*cmd_line_put_char)(rx_char);
        return;
    }
}

bool_t cmd_line_strtol(const char* str, long* val, long min, long max)
{
    long  i;
    char* end;

    i = strtol(str,&end,0);

    if(  (end == str) || (*end != '\0')  )
    {
        return FALSE;
    }

    if((i < min) || (i > max) )
    {
        return FALSE;
    }
    else
    {
        *val = i;
        return TRUE;
    }
}



/* _____LOG__________________________________________________________________ */
/*

 2008/08/01 : Pieter.Conradie
 - Created
 
 2010/02/23 : Neels Kruger
 - Fixed bug in cmd_line_invoke(...) where incorrect arguments where passed
   to handler function in argv[].
 
 2010/04/16 : Pieter.Conradie
 - Changed base in cmd_line_strtol(...) to 0 to support hexidecimal and octal
   numbers.
   
*/
