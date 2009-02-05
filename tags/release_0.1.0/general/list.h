#ifndef __LIST_H__
#define __LIST_H__
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
    
    Title:          Linked List
    Author(s):      Pieter.Conradie
    Creation Date:  2008/11/27
    Revision Info:  $Id: list.h 1 2008-12-02 07:02:22Z Pieter.Conradie $

============================================================================= */

/** 
 *  @ingroup GENERAL
 *  @defgroup ARRAY_LIST list.h : Linked List
 *
 *  A doubly (forwards and backwards) linked list using pointers.
 *  
 *  Files: list.h & list.c
 *  
 *  @see http://en.wikipedia.org/wiki/Linked_list
 * 
 *  Graphical example:
 *  @code
 *  No items in list:
 * 
 *  [first = NULL] [last = NULL]
 * 
 *  One item in list:
 * 
 *  [first] -> [prev = NULL][next = NULL] <- [last]
 * 
 *  Two items in list:
 * 
 *  [first] -> [prev = NULL][next] <-> [prev][next = NULL]  <- [last]
 * 
 *  Three items in list:
 * 
 *  [first] -> [prev = NULL][next] <-> [prev][next] <-> [prev][next = NULL]  <- [last]
 *  @endcode
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stddef.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Link structure that must be at the head of each item in the list
typedef struct list_item_s
{
    struct list_item_s *next_item;
    struct list_item_s *previous_item;
} list_item_t;

/// Linked list structure
typedef struct
{
    struct list_item_s *first_item;        ///< Pointer to first item in the list
    struct list_item_s *last_item;         ///< Pointer to last item in the list
    size_t              nr_of_items;       ///< Counter for number of items in the list
    size_t              max_nr_of_items;   ///< Maximum number of items allowed in list; 0 means no limit
} list_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 * Initialises a linked list structure
 * 
 * @param list              Pointer to the linked list
 * @param max_nr_of_items   Maximum number of items allowed in list; 0 means no limit
 */
extern void list_init(list_t *list,
                      size_t max_nr_of_items);

/** 
 * Initialises a list item.
 * 
 * Initialises the item structure to indicate that it is not in the list.
 * @sa list_item_in_list()
 * 
 * @param list      Pointer to the linked list
 * @param item      Pointer to specified item
 */
extern void list_item_init(list_t      *list,
                           list_item_t *item);

/** 
 * See if the list is empty
 * 
 * @param list      Pointer to the linked list
 * 
 * @return TRUE     List is empty 
 * @return FALSE    List contains one or more items
 */
extern bool_t list_is_empty(list_t *list);

/** 
 * See if the list is full
 * 
 * @param list      Pointer to the linked list
 * 
 * @retval TRUE     The list is full
 * @retval FALSE    The list is not full, or there is no limit (max_nr_of_items = 0)
 */
extern bool_t list_is_full(list_t *list);

/** 
 * Get the number of items in the list
 * 
 *  @param list     Pointer to the linked list
 * 
 *  @returns size_t The number of items in the list 
 */
extern size_t list_nr_of_items(list_t *list);

/** 
 * Get a pointer to the first item in the list
 * 
 * @param list     Pointer to the linked list
 * 
 * @returns list_item_t*    Pointer to the first item in the list;
 *                          NULL will be returned if the list is empty.
 */
extern list_item_t *list_first_item(list_t *list);

/** 
 * Get a pointer to the last item in the list
 * 
 * param list     Pointer to the linked list
 * 
 * @returns list_item_t*    Pointer to the last item in the list;
 *                          NULL will be returned if the list is empty.
 */
extern list_item_t *list_last_item(list_t *list);

/** 
 * Get a pointer to the next item in the list (after the specified item).
 * 
 * param list     Pointer to the linked list
 * 
 * @returns list_item_t*    Pointer to the next item in the list;
 *                          NULL will be returned if the specified item is the
 *                          last one in the list.
 */
extern list_item_t *list_next_item(list_t      *list,
                                   list_item_t *item);

/** 
 * Get a pointer to the previous item in the list (before the specified item).
 * 
 * * param list     Pointer to the linked list
 * 
 * @returns list_item_t*    Pointer to the next item in the list;
 *                          NULL will be returned if the specified item is the
 *                          first one in the list.
 */
extern list_item_t *list_previous_item(list_t      *list,
                                       list_item_t *item);

/** 
 * Insert item to the start of the list.
 * 
 * @param list      Pointer to the linked list
 * @param item      Item to be inserted
 * 
 * @retval TRUE     Item has been inserted
 * @retval FALSE    List is full
 */
extern bool_t list_add_to_start(list_t      *list,
                                list_item_t *item);

/** 
 * Add item to the end of the list.
 * 
 * @param list      Pointer to the linked list
 * @param item      Item to be inserted
 * 
 * @retval TRUE     Item has been inserted
 * @retval FALSE    List is full
 */
extern bool_t list_add_to_end(list_t      *list,
                              list_item_t *item);

/** 
 * Remove first item from the list
 * 
 * @param list      Pointer to the linked list
 * 
 * @return list_item_t* Pointer to the (old) first item;
 *                      NULL will be returned if the list is empty.
 */
extern list_item_t *list_remove_first_item(list_t *list);

/** 
 * Remove last item from the list
 * 
 * @param list      Pointer to the linked list
 * 
 * @return list_item_t* Pointer to the (old) last item;
 *                      NULL will be returned if the list is empty.
 */
extern list_item_t *list_remove_last_item(list_t *list);

/** 
 * Remove item from the list
 * 
 * @param list      Pointer to the linked list
 * @param item      Item to be removed from the list
 * 
 */
extern void list_remove_item(list_t      *list,
                             list_item_t *item);

/** 
 * See if item is in the list
 * 
 * @param list      Pointer to the linked list
 * @param item      Pointer to specified item
 * 
 * @retval TRUE     Item is in the list
 * @retval FALSE    Item is not in the list
 */
extern bool_t list_item_in_list(list_t      *list,
                                list_item_t *item);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
