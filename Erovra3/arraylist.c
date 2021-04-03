#pragma once
#include "arraylist.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

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

void* arraylist_get(struct arraylist* list, int index)
{
	return list->data + index * list->typeSize;
}

/* copies the data at the pointer to a new spot */
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

void arraylist_put(struct arraylist* list, int index, void* data)
{
	memcpy(list->data + index * list->typeSize, data, list->typeSize);
}
