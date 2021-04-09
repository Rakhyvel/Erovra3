/*	arraylist.h

	Date: 4/2/21
	Author: Joseph Shimel
*/

#pragma once

// Some macros that automatically cast type
#define ARRAYLIST_GET(list, index, type)		((type*)(Arraylist_Get(list, index)))
#define ARRAYLIST_POP(list, type)				((type*)(Arraylist_Pop(list)))
#define ARRAYLIST_GET_DEREF(list, index, type)	*((type*)(Arraylist_Get(list, index)))
#define ARRAYLIST_POP_DEREF(list, type)			*((type*)(Arraylist_Pop(list)))

typedef struct arraylist {
	char* data;
	int typeSize;
	int size;
	int capacity;
} Arraylist;

struct arraylist* Arraylist_Create(int initSize, size_t typeSize);

void Arraylist_Add(struct arraylist* list, void* data);
void Arraylist_Put(struct arraylist* list, int index, void* data);
void* Arraylist_Get(struct arraylist* list, int index);
void* Arraylist_Pop(struct arraylist* list);