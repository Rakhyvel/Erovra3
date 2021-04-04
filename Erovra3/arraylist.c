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
#include <stdlib.h>
#include <string.h>

/*
	Creates an arraylist with a given type size */
struct arraylist* arraylist_create(int typeSize)
{
	struct arraylist* retval = malloc(sizeof(struct arraylist));
	if (!retval) 
	{
		PANIC("Memory error");
		exit(1);
	}
	retval->typeSize = typeSize;
	retval->size = 0;
	retval->capacity = 10;
	retval->data = calloc(retval->capacity * retval->typeSize, sizeof(char));
	return retval;
}

/*
	Returns a pointer to within the arraylist at a given index */
void* arraylist_get(struct arraylist* list, int index)
{
	return list->data + (index * list->typeSize);
}

/*
	Returns a pointer to last element in arraylist, decrements size
	
	Doesn't reduce memory pool size on purpose */
void* arraylist_pop(struct arraylist* list)
{
	if (list->size == 0)
	{
		PANIC("List is too small to be popped");
	}
	else
	{
		list->size--;
		return list->data + ((long long)list->size + 1) * list->typeSize;
	}
}

/* 
	copies the data at the pointer to a new spot 
	also functions as a push method
*/
void arraylist_add(struct arraylist* list, void* data)
{
	if (list->size >= list->capacity) 
	{
		void* res = realloc(list->data, 2 * list->size * list->typeSize);
		printf("Prev ptr: %p new ptr: %p\n", list->data, res);
		if (res != NULL && res != list->data)
		{
			list->data = res;
		}
		list->capacity = list->size * 2;
	}
	arraylist_put(list, list->size, data);
	list->size++;
}

/*
	Copies data to an index */
void arraylist_put(struct arraylist* list, int index, void* data)
{
	memcpy(list->data + index * list->typeSize, data, list->typeSize);
}
