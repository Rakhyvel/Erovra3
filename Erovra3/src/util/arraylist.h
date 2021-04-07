/*	arraylist.h

	Date: 4/2/21
	Author: Joseph Shimel
*/

#pragma once

// Some macros that automatically cast type
#define ARRAYLIST_GET(list, index, type)		((type*)(arraylist_get(list, index)))
#define ARRAYLIST_POP(list, type)				((type*)(arraylist_pop(list)))
#define ARRAYLIST_GET_DEREF(list, index, type)	*((type*)(arraylist_get(list, index)))
#define ARRAYLIST_POP_DEREF(list, type)			*((type*)(arraylist_pop(list)))

struct arraylist {
	char* data;
	int typeSize;
	int size;
	int capacity;
};

struct arraylist* arraylist_create(int initSize, size_t typeSize);

void arraylist_add(struct arraylist* list, void* data);
void arraylist_put(struct arraylist* list, int index, void* data);
void* arraylist_get(struct arraylist* list, int index);
void* arraylist_pop(struct arraylist* list);