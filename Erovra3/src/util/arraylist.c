/*	arraylist.c

	An arraylist is a pool of memory that grows as more memory is added to it.
	Acts like a regular list. Memory is copied byte by byte when added, memory
	address from within the list are returned.

	Arraylists have a type size, which is the number of bytes each element in
	the pool is.

	Date: 4/2/21
	Author: Joseph Shimel
*/

#pragma once
#include "arraylist.h"
#include "debug.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
	Creates an arraylist with a given type size */
struct arraylist* Arraylist_Create(int initSize, int typeSize)
{
    struct arraylist* retval = malloc(sizeof(struct arraylist));
    if (!retval) {
        PANIC("Memory error");
    }
    retval->typeSize = typeSize;
    retval->size = 0;
    retval->capacity = initSize;
    retval->data = calloc(retval->capacity * retval->typeSize, sizeof(char));
    if (!retval->data) {
        PANIC("Memory error");
    }
    return retval;
}

void Arraylist_Destroy(struct arraylist* list)
{
    if (list == NULL) {
        printf("WARNING: list was NULL!");
        return;
    }
    free(list->data);
    free(list);
}

/*
	Returns a pointer to within the arraylist at a given index */
void* Arraylist_Get(struct arraylist* list, int index)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (index > list->size) {
        PANIC("List index out of bounds. List size is %d, index was %d", list->size, index);
    } else if (index < 0) {
        PANIC("Negative index. Index was %d", index);
    }
    return list->data + (index * list->typeSize);
}

/*
	Returns a POINTER TO THE DATA of the last element in arraylist, decrements size
	
	Doesn't reduce memory pool size on purpose */
void* Arraylist_Pop(struct arraylist* list)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (list->size == 0) {
        PANIC("List is too small to be popped");
    }

    list->size--;
    return list->data + ((long long)list->size) * list->typeSize;
}

/* 
	copies the data at the pointer to the end of an arraylist
	also functions as a push method */
void Arraylist_Add(struct arraylist* list, void* data)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (data == NULL) {
        PANIC("Data is NULL");
    }

    if (list->size >= list->capacity) {
        void* res = realloc(list->data, 2 * list->size * list->typeSize); // FIXME: heap error
        if (res != NULL) {
            list->data = res;
        } else {
            PANIC("Mem err, asked to reallocate %d", 2 * list->size * list->typeSize);
        }
        list->capacity = list->size * 2;
    }
    Arraylist_Put(list, list->size, data);
    list->size++;
}

/*
	Copies data to an index */
void Arraylist_Put(struct arraylist* list, int index, void* data)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (data == NULL) {
        PANIC("Data is NULL");
    } else if (index > list->size) {
        PANIC("Index out of bounds. Index was %d, list size is %d", index, list->size);
    } else if (index < 0) {
        PANIC("Negative index. Index was %d", index);
    }

    memcpy(list->data + index * list->typeSize, data, list->typeSize);
}

bool Arraylist_Contains(struct arraylist* list, void* data)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (data == NULL) {
        PANIC("Data is NULL");
    }

    for (int i = 0; i < list->size; i++) {
        if (!memcmp(list->data + (list->typeSize * i), data, list->typeSize)) {
            return true;
        }
    }
    return false;
}

/*	Sets the size of the arraylist. Resizes the arraylist if it is too small. 
 *	Does nothing if list type size is 0. Does nothing if list size is already 
 *	larger than given size.
 *
 *	@param list	List to assert size of
 *  @param size	Size to assert.
 */
void Arraylist_AssertSize(struct arraylist* list, int size)
{
    if (list == NULL) {
        PANIC("List is NULL");
    }
    // Exit if list is ethereal
    if (list->typeSize == 0) {
        return;
    }

    if (size > list->size) {
        list->size = size;
        list->capacity = list->size * 2;
        void* res = realloc(list->data, list->capacity * list->typeSize); // FIXME: heap error
        if (res != NULL) {
            list->data = res;
        } else {
            PANIC("Mem err, asked to reallocate %d %d", list->capacity, list->typeSize);
        }
    }
}