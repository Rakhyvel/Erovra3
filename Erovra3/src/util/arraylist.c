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
struct arraylist* Arraylist_Create(int initSize, size_t typeSize)
{
    struct arraylist* retval = malloc(sizeof(struct arraylist));
    if (!retval) {
        PANIC("Memory error");
        exit(1);
    }
    retval->typeSize = typeSize;
    retval->size = 0;
    retval->capacity = initSize;
    retval->data = calloc(retval->capacity * retval->typeSize, sizeof(char));
    return retval;
}

/*
	Returns a pointer to within the arraylist at a given index */
void* Arraylist_Get(struct arraylist* list, int index)
{
    return list->data + (index * list->typeSize);
}

/*
	Returns a pointer to last element in arraylist, decrements size
	
	Doesn't reduce memory pool size on purpose */
void* Arraylist_Pop(struct arraylist* list)
{
    if (list->size == 0) {
        PANIC("List is too small to be popped");
    } else {
        list->size--;
        return list->data + ((long long)list->size) * list->typeSize;
    }
}

/* 
	copies the data at the pointer to the end of an arraylist
	also functions as a push method */
void Arraylist_Add(struct arraylist* list, void* data)
{
    if (list->size >= list->capacity) {
        void* res = realloc(list->data, 2 * list->size * list->typeSize); // FIXME: heap error
        if (res != NULL && res != list->data) {
            list->data = res;
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
    memcpy(list->data + index * list->typeSize, data, list->typeSize);
}

bool Arraylist_Contains(struct arraylist* list, void* data)
{
    for (int i = 0; i < list->size; i++) {
        if (!memcmp(list->data + (list->typeSize * i), data, list->typeSize)) {
            return true;
        }
    }
    return false;
}