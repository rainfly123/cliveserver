/*
Copyright (C) 2014 xiechc@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

list.h

Generic list management functions.

*/
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

/*******************************************************************************
* Global variables                                                             *
*******************************************************************************/
/*******************************************************************************
* Global functions                                                             *
*******************************************************************************/


void ListFree(List_t *list, void (*destructor)(void *data))
{
    ListEntry_t *entry;
    ListEntry_t *next;
    for (entry = list->head; entry != NULL; entry = next)
    {
        next = entry->next;
        if (destructor)
        {
            destructor(entry->data);
        }
        free(entry);
    }
    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

bool ListAdd(List_t *list, void *data)
{
    ListIterator_t iterator = {list, list->tail};
    return ListInsertAfterCurrent(&iterator,data);
}

bool ListInsertAfterCurrent(ListIterator_t *iterator, void *data)
{
    ListEntry_t *entry;

    entry = calloc(1, sizeof(ListEntry_t));
    if (entry == NULL)
    {
        return false;
    }
    entry->data = data;
    if (iterator->current == NULL) 
    {
        entry->next = NULL;
        entry->prev = iterator->list->tail;
        if (entry->prev) {
                entry->prev->next = entry;
        }
    }
    else 
    {
        entry->next = iterator->current->next;
        entry->prev = iterator->current;
        iterator->current->next = entry;
        if (entry->next) 
        {
            entry->next->prev = entry;
        }
    }
    if (iterator->list->head == NULL)
    {
        iterator->list->head = entry;
    }
    if (entry->next == NULL) 
    {
        iterator->list->tail = entry;
    }
    iterator->list->count ++;
    return true;
}

bool ListInsertBeforeCurrent(ListIterator_t *iterator, void *data)
{
    ListEntry_t *entry;
    entry = calloc(1, sizeof(ListEntry_t));
    if (entry == NULL)
    {
        return false;
    }
    entry->data = data;
    entry->next = iterator->current;
    if (iterator->current == NULL) 
    {
        entry->prev = iterator->list->tail;
        iterator->list->tail = entry;
    }
    else
    {
        entry->prev = iterator->current->prev;
        entry->next->prev = entry;
    }
    if (iterator->current == iterator->list->head)
    {
        iterator->list->head = entry;
    }
    if (entry->prev) 
    {
        entry->prev->next = entry;
    }
    iterator->list->count ++;
    return true;
}

bool ListRemove(List_t *list, void *data)
{
    ListIterator_t iterator;
    for ( ListIterator_Init(iterator, list); ListIterator_MoreEntries(iterator); ListIterator_Next(iterator))
    {
        if (data == ListIterator_Current(iterator))
        {
            ListRemoveCurrent(&iterator);
            return true;
        }
    }
    return false;
}

void ListRemoveCurrent(ListIterator_t *iterator)
{
    List_t *list = iterator->list;
    ListEntry_t *entry = iterator->current;
    iterator->current = entry->next;
    if (entry == list->head)
    {
        list->head = entry->next;
    }
    if (entry == list->tail)
    {
        list->tail = entry->prev;
    }
    if (entry->prev)
    {
        entry->prev->next = entry->next;
    }
    if (entry->next)
    {
        entry->next->prev = entry->prev;
    }
    free(entry);
    iterator->list->count --;
}

void ListDump(List_t *list)
{
    ListIterator_t iterator;

    printf( "Dumping list %p (%d entries)\n", list, list->count);
    for ( ListIterator_Init(iterator, list); ListIterator_MoreEntries(iterator); ListIterator_Next(iterator))
    {
        printf("Current = %10p prev = %10p  next = %10p data = %10p\n",
        iterator.current, iterator.current->prev, iterator.current->next, iterator.current->data);
    }
    printf("End of dump\n");
}
