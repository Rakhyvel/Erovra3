/*	arraylist.h

	Date: 4/2/21
	Author: Joseph Shimel
*/

#pragma once
#include <stdlib.h>
#include <stdbool.h>
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

struct arraylist* Arraylist_Create(int initSize, int typeSize);
void Arraylist_Destroy(struct arraylist* list);

void Arraylist_Add(struct arraylist* list, void* data);
void Arraylist_Remove(struct arraylist* list, int index);
int Arraylist_IndexOf(struct arraylist* list, void* data);
void Arraylist_Put(struct arraylist* list, int index, void* data);
void* Arraylist_Get(struct arraylist* list, int index);
void* Arraylist_Pop(struct arraylist* list);
bool Arraylist_Contains(struct arraylist* list, void* data);
void Arraylist_AssertSize(struct arraylist* list, int size);