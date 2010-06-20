#ifndef __KBD_MATRIX_H__
#define __KBD_MATRIX_H__
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
    
    Title:          Matrix Keyboard module
    Author(s):      Pieter.Conradie
    Creation Date:  2008/11/21
    Revision Info:  $Id$

============================================================================= */

/** 
 *  @ingroup DRIVERS
 *  @defgroup KBD_MATRIX kbd_matrix.h : Matrix Keyboard module
 *
 *  Full featured matrix (row/column) keyboard scanning module
 *  
 *  Files: kbd_matrix.h & kbd_matrix.c
 *  
 *  This module implements or makes provision for:
 *  - Optimised matrix row/column key scanning
 *  - Debouncing of key press and release
 *  - n-key rollover
 *  - Detection and elimination of "phantom" keys
 *  - Key repetition (typematic)
 *  
 *  @see
 *  - http://en.wikipedia.org/wiki/Computer_keyboard
 *  - http://en.wikipedia.org/wiki/Rollover_(key)
 *  - http://www.dribin.org/dave/keyboard/one_html/
 *  
 *  @note This module is based on "kbmatrix.h" and "kbmatrix.c" found in the
 *        <A href="http://www.atmel.com/dyn/products/tools_card.asp?tool_id=4343">Atmel AT91 Software package</a>
 *  
 * License:
 * @code
 * ----------------------------------------------------------------------------
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
 * @endcode
 * @{ 
 */ 

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
#include "list.h"

/* _____DEFINITIONS _________________________________________________________ */
#ifndef KBD_MATRIX_MAX_NR_KEYS
/// Maximum number of keys that can be handled by the module.
#define KBD_MATRIX_MAX_NR_KEYS  56
#if ((KBD_MATRIX_MAX_NR_KEYS % 8) != 0)
#error "KBD_MATRIX_MAX_NR_KEYS must be a multiple of 8"
#endif
#endif

#ifndef KBD_MATRIX_NR_SAMPLES
/// Number of samples that are necessary to debounce a keypress.
#define KBD_MATRIX_NR_SAMPLES  4
#endif

#ifndef KBD_MATRIX_MAX_NR_KEYS_PRESSED
/// Maximum number of keys that may be pressed simultaneously
#define KBD_MATRIX_MAX_NR_KEYS_PRESSED 4
#endif

#ifndef KBD_DELAY_BEFORE_REPEATING_KEY
/// Delay before a pressed key will be repeated
#define KBD_DELAY_BEFORE_REPEATING_KEY 200
#endif

#ifndef KBD_KEY_REPEAT_DELAY
/// Delay between pressed key repeats
#define KBD_KEY_REPEAT_DELAY 20
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Key event types
typedef enum
{
    KBD_MATRIX_EVENT_RELEASED = 0,  ///< Key has been released
    KBD_MATRIX_EVENT_PRESSED  = 1   ///< Key has been pressed
} kbd_matrix_event_t;

/**
 * Function that will be called when a key event has taken place
 * 
 * @param[in] row       Row address of key event 
 * @param[in] col       Column address of key event 
 * @param[in] event     Key event type: PRESSED or RELEASED
 *  
 */
typedef void (*kbd_matrix_on_key_event_t)(u8_t row, u8_t col, kbd_matrix_event_t event);

/**
 * Function that will be called to set a row output 
 *  
 * @param[in] row       Specifies which row that should be set
 * @param[in] active    Specifies that row should be activated 
 *                      (normally active low)
 */
typedef void (*kbd_matrix_set_row_t)(u8_t row, bool_t active);

/**
 * Function that will be called to read a column input
 *
 * @retval TRUE     Key is being pressed
 * @retval FALSE    Key is not pressed (released)
 */
typedef bool_t (*kbd_matrix_get_col_t)(u8_t col);

/**
 * Function that will be called for row column scanning delay. 
 *  
 * If a row is set to active, there is a certain delay before the columns should 
 * be sampled. 
 *  
 * If a row is set to inactive, there is a certain delay before the columns 
 * return to their inactive state. 
 *  
 * @param[in] active    Specifies that row should be activated 
 *                      (normally active low)
 */
typedef void (*kbd_matrix_delay_t)(bool_t active);

/// Key state bit mask (1 = key pressed; 0 = key released)
typedef struct
{
    u8_t bit_mask[KBD_MATRIX_MAX_NR_KEYS/8];
} kbd_matrix_key_state_t;

/// Linked list structure that describes which key has been pressed
typedef struct
{
    list_item_t key_list_item;
    u8_t        row;
    u8_t        col;
} kbd_matrix_key_t;

typedef struct
{
    /// Number of rows in matrix
    u8_t                      nr_of_rows;
    /// Number of columns in matrix
    u8_t                      nr_of_columns;

    /// Bit mask that stores the current state of the keys
    kbd_matrix_key_state_t    key_state;

    /// Array of bit masks that stores the key state history
    kbd_matrix_key_state_t    key_history[KBD_MATRIX_NR_SAMPLES];

    /// Wrapping index (used as a ring buffer)
    u8_t                      key_history_index;

    /// Linked list of pressed keys
    list_t                    key_press_list;

    /// Storage for linked list to remember which keys are pressed
    kbd_matrix_key_t          key_press_array[KBD_MATRIX_MAX_NR_KEYS_PRESSED];

    // Counter to determine if a pressed key must repeated (typematic)
    u16_t                     key_repeat_counter;

    /// Function handler to write a row output
    kbd_matrix_set_row_t      set_row;

    /// Function handler to read a column input
    kbd_matrix_get_col_t      get_col;

    /// Function handler to read a column input
    kbd_matrix_delay_t        delay;

    /// Function that will be called if a key is pressed or released
    kbd_matrix_on_key_event_t on_key_event_handler;
} kbd_matrix_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
extern void kbd_matrix_init(kbd_matrix_t                *matrix,
                            u8_t                        nr_of_rows,
                            u8_t                        nr_of_columns,
                            kbd_matrix_set_row_t        set_row,
                            kbd_matrix_get_col_t        get_col,
                            kbd_matrix_delay_t          delay,
                            kbd_matrix_on_key_event_t   on_key_event_handler);

extern void kbd_matrix_scan(kbd_matrix_t *matrix);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
