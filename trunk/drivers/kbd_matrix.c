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
    
    Title:          Matrix Keyboard module
    Author(s):      Pieter.Conradie
    Creation Date:  2008/11/21
    Revision Info:  $Id$

============================================================================= */

/*  
 *  @note this module is built on "kbmatrix.h" and "kbmatrix.c" found in the Atmel
 *        AT91 Software package:
 *        (http://www.atmel.com/dyn/products/tools_card.asp?tool_id=4343)
 *        See license below:
 *  
 * @{ 
 */

/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "kbd_matrix.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void kbd_key_state_init(kbd_matrix_key_state_t *key_state)
{
    memset(key_state, 0, sizeof(kbd_matrix_key_state_t));
}

static void kbd_key_state_copy(kbd_matrix_key_state_t       *key_state_to,
                               const kbd_matrix_key_state_t *key_state_from)
{
    memcpy(key_state_to, key_state_from, sizeof(kbd_matrix_key_state_t));
}

static void kbd_key_state_set(kbd_matrix_key_state_t *key_state,
                              u8_t                   key,
                              u8_t                   state)    
{
    u8_t byte = key / 8;
    u8_t bit  = key % 8;

    if (state)
    {
        key_state->bit_mask[byte] |= (1<<bit);
    }
    else
    {
        key_state->bit_mask[byte] &= ~(1<<bit);
    }
}

static u8_t kbd_key_state_get(kbd_matrix_key_state_t *key_state,
                              u8_t                   key)
{
    u8_t byte = key / 8;
    u8_t bit  = key % 8;

    u8_t state = (key_state->bit_mask[byte] >> bit) & 1;

    return state;
}

static void kbd_key_state_and(kbd_matrix_key_state_t *key_x,
                              kbd_matrix_key_state_t *key_y,
                              kbd_matrix_key_state_t *key_result)
{
    size_t i;

    for (i=0; i < sizeof(kbd_matrix_key_state_t); i++)
    {
        key_result->bit_mask[i] = key_x->bit_mask[i] & key_y->bit_mask[i];
    }
}

static void kbd_key_state_or (kbd_matrix_key_state_t *key_x,
                              kbd_matrix_key_state_t *key_y,
                              kbd_matrix_key_state_t *key_result)
{
    size_t i;

    for (i=0; i < sizeof(kbd_matrix_key_state_t); i++)
    {
        key_result->bit_mask[i] = key_x->bit_mask[i] | key_y->bit_mask[i];
    }
}

static void kbd_key_state_xor(kbd_matrix_key_state_t *key_x,
                              kbd_matrix_key_state_t *key_y,
                              kbd_matrix_key_state_t *key_result)
{
    size_t i;

    for (i=0; i < sizeof(kbd_matrix_key_state_t); i++)
    {
        key_result->bit_mask[i] = key_x->bit_mask[i] ^ key_y->bit_mask[i];
    }
}

static bool_t kbd_matrix_phantom_key(list_t *key_press_list)
{
    kbd_matrix_key_t *key_press;
    kbd_matrix_key_t *key_press_compare;
    bool_t           same_row_flag, same_col_flag;
    

    // Start at first item in the list
    key_press = (kbd_matrix_key_t *)list_first_item(key_press_list);

    while(key_press != NULL)
    {
        // Reset flags
        same_row_flag = FALSE;
        same_col_flag = FALSE;

        // Compare with other keys in the list
        key_press_compare = (kbd_matrix_key_t *)list_first_item(key_press_list);
        while(key_press_compare != NULL)
        {
            // Do not compare key with itself
            if(key_press_compare != key_press)
            {
                // See if key is in the same row
                 if(key_press->row == key_press_compare->row)
                {
                    same_row_flag = TRUE;
                }
                // See if key is in the same column
                if(key_press->col == key_press_compare->col)
                {
                    same_col_flag = TRUE;
                }
                /*
                 * See if a key has been detected in the same row 
                 * and another key has been detected in the same column
                 */
                if( (same_row_flag == TRUE) && (same_col_flag == TRUE) )
                {
                    // Phantom key detected
                    return TRUE;
                }
            }
            // Next item
            key_press_compare = (kbd_matrix_key_t *)list_next_item(key_press_list, &key_press_compare->key_list_item);
        }

        // Next item
        key_press = (kbd_matrix_key_t *)list_next_item(key_press_list, &key_press->key_list_item);
    }

    return FALSE;
}

static void kbd_matrix_on_key_event(kbd_matrix_t       *matrix,
                                    u8_t               row,
                                    u8_t               col,
                                    kbd_matrix_event_t event)
{
    list_t      *key_press_list = &(matrix->key_press_list);
    list_item_t *key_press_item;
    kbd_matrix_key_t *key_press;
    u8_t             i;    

    if(event == KBD_MATRIX_EVENT_PRESSED)
    {
        // See if key pressed list is full
        if(list_is_full(key_press_list))
        {
            // Ignore extra key presses
            return;
        }
        
        // Get an unused structure to add to the list
        for(i=0; i<KBD_MATRIX_MAX_NR_KEYS_PRESSED; i++)
        {
            key_press = &(matrix->key_press_array[i]);

            if(list_item_in_list(key_press_list, &(key_press->key_list_item)) == FALSE)
            {
                // Add key to list
                key_press->row               = row;
                key_press->col               = col;

                list_add_to_end(key_press_list, &(key_press->key_list_item));
				break;
            }
        }

        // See if phantom key detection should be performed
        if(list_nr_of_items(key_press_list) >= 3)
        {
            // See if key forms the third side of a rectangle
            if(kbd_matrix_phantom_key(key_press_list))
            {
                // Remove phantom key from the list
                key_press = (kbd_matrix_key_t *)list_remove_last_item(key_press_list);
                return;
            }
        }

        // Reset counter
        matrix->key_repeat_counter = KBD_DELAY_BEFORE_REPEATING_KEY;

        // Call event handler
        matrix->on_key_event_handler(row, col, event);
    }

    if(event == KBD_MATRIX_EVENT_RELEASED)
    {
        // Start at first item in the list
        key_press_item = list_first_item(key_press_list);
        while(key_press_item != NULL)
        {
            // Access data of list item
            key_press = (kbd_matrix_key_t *)key_press_item;

            // See if key is in the list
            if(  (key_press->row == row)
               &&(key_press->col == col)  )
            {
                // Remove key from the list
                list_remove_item(key_press_list, key_press_item);

                // Call event handler
                matrix->on_key_event_handler(row, col, event);

                break;
            }
            // Next item
            key_press_item = list_next_item(key_press_list, key_press_item);
        }        
    }    
}

/** 
 * Computes the new debounced key state and trigger key events if necessary.
 *
 * The debouncing is done both when the key is pressed and when it is released.
 *  
 * @param matrix    Pointer to matrix instance
 * 
 * @retval TRUE     Changes have been detected
 * @retval FALSE    No change have been detected
 */
static void kbd_matrix_debounce(kbd_matrix_t *matrix)
{
    static kbd_matrix_key_state_t  pressed;
    static kbd_matrix_key_state_t  released;
    static kbd_matrix_key_state_t  new;
    static kbd_matrix_key_state_t  changed;

    u8_t   i;
    u8_t   nr_of_keys;

    /*
     * Debounce pressed keys
     *
     * pressed[x] will be 1 if all key_history[x] samples are 1
     * pressed[x] will be 0 if any key_history[x] sample is 0
     */
    kbd_key_state_copy(&pressed, &(matrix->key_history[0]));
    for (i=1; i < KBD_MATRIX_NR_SAMPLES; i++)
    {
        kbd_key_state_and(&pressed, &(matrix->key_history[i]), &pressed);
    }

    /*
     * Debounce released keys
     *
     * released[x] will be 0 if all key_history[x] samples are 0
     * released[x] will be 1 if any key_history[x] sample is 1
     */
    // Debounce released keys
    kbd_key_state_copy(&released, &(matrix->key_history[0]));
    for (i=1; i < KBD_MATRIX_NR_SAMPLES; i++)
    {
        kbd_key_state_or(&released, &(matrix->key_history[i]), &released);
    }

    // Compute new key status
    kbd_key_state_or(&(matrix->key_state), &pressed, &new);
    kbd_key_state_and(&new, &released, &new);

    // Compare with existing status
    kbd_key_state_xor(&(matrix->key_state), &new, &changed);
    
    // Process each pending event
    nr_of_keys = matrix->nr_of_rows * matrix->nr_of_columns;
    for (i=0; i < nr_of_keys; i++)
	{    
        if (kbd_key_state_get(&changed, i))
        {
            // Deterimine if a key has been pressed or released
            kbd_matrix_event_t event;
            if(kbd_key_state_get(&new, i))
            {
                event = KBD_MATRIX_EVENT_PRESSED;
            }
            else
            {
                event = KBD_MATRIX_EVENT_RELEASED;
            }

            // Handle key event
            kbd_matrix_on_key_event(matrix,
                                    i / matrix->nr_of_columns,
                                    i % matrix->nr_of_columns,
                                    event);
        }
    }
    // Save new key state
    kbd_key_state_copy(&(matrix->key_state), &new);
}

/** 
 * Enable all rows and see if any key has been pressed.
 * 
 * @param matrix    Pointer to matrix instance 
 *  
 * @retval TRUE     A key has been pressed 
 * @retval FALSE    No key has been pressed
 */
static bool_t kbd_matrix_key_pressed(kbd_matrix_t *matrix)
{
    u8_t   row;
    u8_t   col;
    bool_t key_pressed_flag = FALSE;

    // Enable all rows
    for (row=0; row < matrix->nr_of_rows; row++)
    {
        matrix->set_row(row, TRUE);      
    }
    // Wait before columns must be sampled
    matrix->delay(TRUE);

    // Scan each column
    for (col=0; col < matrix->nr_of_columns; col++)
    {
        if(matrix->get_col(col))
        {
            // A Key has been pressed
            key_pressed_flag = TRUE;
            break;
        }
    }
    // Disable all rows
    for (row=0; row < matrix->nr_of_rows; row++)
    {
        matrix->set_row(row, FALSE);
    }
    // Wait for columns to return to their inactive state
    matrix->delay(FALSE);

    return key_pressed_flag;
}

/** 
 * Retrieves a new sample set by reading the current state of the keyboard.
 * 
 * @param matrix    Pointer to matrix instance
 */
static void kbd_matrix_sample(kbd_matrix_t *matrix)
{
    // Pointer to array where new sample set must be stored
    kbd_matrix_key_state_t *key_state = &(matrix->key_history[matrix->key_history_index]);

    u8_t   row;
    u8_t   col;
    u8_t   key;

    // Clear array
    kbd_key_state_init(key_state);

    // See if a key has been pressed
    if(kbd_matrix_key_pressed(matrix))
    {    
        // Determine which key(s) has been pressed
        key = 0;
        for (row=0; row < matrix->nr_of_rows; row++)
        {
            // Enable row
            matrix->set_row(row, TRUE);
            // Wait before columns must be sampled
            matrix->delay(TRUE);
    
            // Scan each column
            for (col=0; col < matrix->nr_of_columns; col++)
            {
                // See if key is being pressed
                if(matrix->get_col(col))
                {
                    // Set bit
                    kbd_key_state_set(key_state, key, 1);
                }
                // Next key
                key++;
            }
            // Disable row
            matrix->set_row(row, FALSE);
            // Wait for columns to return to their inactive state
            matrix->delay(FALSE);
        }
    }    

    // Update history index
    if(++(matrix->key_history_index) == KBD_MATRIX_NR_SAMPLES)
    {
        // Wrap index
        matrix->key_history_index = 0;
    }
}

static void kbd_matrix_repeat(kbd_matrix_t *matrix)
{
    list_t      *key_press_list = &(matrix->key_press_list);
    kbd_matrix_key_t *key_press;

    // See if a key(s) must be repeated
    if(list_is_empty(key_press_list) == FALSE)
    {
        // See if counter has expired
        if(matrix->key_repeat_counter == 0)
        {
            // Reset counter
            matrix->key_repeat_counter = KBD_KEY_REPEAT_DELAY;
            // Get last key pressed
            key_press = (kbd_matrix_key_t *)list_last_item(key_press_list);
            // Repeat key
            matrix->on_key_event_handler(key_press->row, key_press->col, KBD_MATRIX_EVENT_PRESSED);
        }
        else
        {
            // Decrement counter
            (matrix->key_repeat_counter)--;
        }
    }
    else
    {
        // Reset counter
        matrix->key_repeat_counter = KBD_DELAY_BEFORE_REPEATING_KEY;
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void kbd_matrix_init(kbd_matrix_t               *matrix,
                     u8_t                       nr_of_rows,
                     u8_t                       nr_of_columns,
                     kbd_matrix_set_row_t       set_row,
                     kbd_matrix_get_col_t       get_col,
                     kbd_matrix_delay_t         delay,
                     kbd_matrix_on_key_event_t  on_key_event_handler)
{
    u8_t i;

    // Initialise structure
    matrix->nr_of_rows           = nr_of_rows;
    matrix->nr_of_columns        = nr_of_columns;
    matrix->set_row              = set_row;
    matrix->get_col              = get_col;
    matrix->delay                = delay;
    matrix->on_key_event_handler = on_key_event_handler;
    matrix->key_repeat_counter   = KBD_DELAY_BEFORE_REPEATING_KEY;

    // Initialise key state array
    kbd_key_state_init(&(matrix->key_state));

    // Initialise history state array(s)
    for(i=0; i < KBD_MATRIX_NR_SAMPLES; i++)
    {
        kbd_key_state_init(&(matrix->key_history[i]));
    }

    // Initialise linked list
    list_init(&(matrix->key_press_list), KBD_MATRIX_MAX_NR_KEYS_PRESSED);

    // Initialise items used for linked list
    for(i=0; i<KBD_MATRIX_MAX_NR_KEYS_PRESSED; i++)
    {
        list_item_init(&(matrix->key_press_list), &(matrix->key_press_array[i].key_list_item));
        matrix->key_press_array[i].row               = 0;
        matrix->key_press_array[i].col               = 0;
    }
}

void kbd_matrix_scan(kbd_matrix_t *matrix)
{
    kbd_matrix_sample(matrix);

    kbd_matrix_debounce(matrix);

    kbd_matrix_repeat(matrix);
}

/* _____LOG__________________________________________________________________ */
/*

 2008/11/21 : Pieter.Conradie
 - Created
   
*/
