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
    
    Title:          Link List
    Author(s):      Pieter.Conradie
    Creation Date:  2008/11/27
    Revision Info:  $Id$

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "list.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void list_init(list_t *list,
               size_t max_nr_of_items)
{
    list->first_item      = NULL;
    list->last_item       = NULL;
    list->nr_of_items     = 0;
    list->max_nr_of_items = max_nr_of_items;
}

void list_item_init(list_t      *list,
                    list_item_t *item)
{
    item->previous_item = NULL;
    item->next_item     = NULL;
}

bool_t list_is_empty(list_t *list)
{
    if(list->first_item == NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool_t list_is_full(list_t *list)
{
    if(list->max_nr_of_items == 0)
    {
        return FALSE;
    }

    if(list->nr_of_items < list->max_nr_of_items)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

size_t list_nr_of_items(list_t *list)
{
    // Return item count
    return (list->nr_of_items);
}

list_item_t *list_first_item(list_t *list)
{
    return list->first_item;
}

list_item_t *list_last_item(list_t *list)
{
    return list->last_item;
}

list_item_t *list_next_item(list_t      *list,
                            list_item_t *item)
{
    return item->next_item;
}

list_item_t *list_previous_item(list_t      *list,
                                list_item_t *item)
{
    return item->previous_item;
}

bool_t list_add_to_start(list_t      *list,
                         list_item_t *item)
{
    if(list_is_full(list))
    {
        return FALSE;
    }

    if(list_is_empty(list))
    {
        // Add first item
        list->first_item    = item;
        list->last_item     = item;
        item->next_item     = NULL;
        item->previous_item = NULL;
    }
    else
    {
        // Insert new item before first item
        item->previous_item             = NULL;
        item->next_item                 = list->first_item;
        list->first_item->previous_item = item;
        list->first_item                = item;
    }

    // Increment item count
    (list->nr_of_items)++;

    return TRUE;
}

bool_t list_add_to_end(list_t      *list,
                       list_item_t *item)
{
    if(list_is_full(list))
    {
        return FALSE;
    }

    if(list_is_empty(list))
    {
        // Add first item
        list->first_item    = item;
        list->last_item     = item;
        item->next_item     = NULL;
        item->previous_item = NULL;
    }
    else
    {
        // Append new item to last item
        item->previous_item        = list->last_item;
        item->next_item            = NULL;
        list->last_item->next_item = item;
        list->last_item            = item;
    }

    // Increment item count
    (list->nr_of_items)++;

    return TRUE;
}

list_item_t *list_remove_first_item(list_t *list)
{
    list_item_t *item = list->first_item;

    // See if list is empty
    if(list_is_empty(list))
    {
        return NULL;
    }

    // See if there is only one item
    if(list->first_item == list->last_item)
    {
        list->first_item = NULL;
        list->last_item  = NULL;
    }
    else
    {
        // The next item become the first one in the list
        list->first_item          = item->next_item;
        item->next_item->previous_item = NULL;
    }

    // Clear links of removed item
    item->previous_item = NULL;
    item->next_item     = NULL;

    // Decrement item count
    (list->nr_of_items)--;

    return item;
}

list_item_t *list_remove_last_item(list_t *list)
{
    list_item_t *item = list->last_item;

    // See if list is empty
    if(list_is_empty(list))
    {
        return NULL;
    }

    // See if there is only one item
    if(list->first_item == list->last_item)
    {
        list->first_item = NULL;
        list->last_item  = NULL;
    }
    else
    {
        // The previous item become the last one in the list
        list->last_item                = item->previous_item;
        item->previous_item->next_item = NULL;        
    }

    // Clear links of removed item
    item->previous_item = NULL;
    item->next_item     = NULL;

    // Decrement item count
    (list->nr_of_items)--;

    return item;
}

void list_remove_item(list_t      *list,
                      list_item_t *item)
{
    // Extra sanity check
    if(list_is_empty(list))
    {
        return;
    }

    // See if this is the first item in the list
    if(item == list->first_item)
    {
        list_remove_first_item(list);
        return;
    }

    // See if this is the last item in the list
    if(item == list->last_item)
    {
        list_remove_last_item(list);
        return;
    }

    // Link previous and next item to each other
    item->previous_item->next_item = item->next_item;
    item->next_item->previous_item = item->previous_item;
    
    // Clear links of item
    item->previous_item = NULL;
    item->next_item     = NULL;

    // Decrement item count
    (list->nr_of_items)--;
}

bool_t list_item_in_list(list_t      *list,
                         list_item_t *item)
{
    // Start at first item in the list
    list_item_t *item_in_list = list->first_item;

    // Search all items in the list
    while(item_in_list != NULL)
    {
        if(item_in_list == item)
        {
            // Item is in the list
            return TRUE;
        }
        // Next item
        item_in_list = item_in_list->next_item;
    }
    // Item is not in the list
    return FALSE;
}

/* _____LOG__________________________________________________________________ */
/*

 2008/11/27 : Pieter.Conradie
 - Created
   
*/
