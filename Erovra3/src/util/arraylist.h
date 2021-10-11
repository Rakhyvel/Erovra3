#ifndef ARRAYLIST_H
#define ARRAYLIST_H
/*	arraylist.h

	Date: 4/2/21
	Author: Joseph Shimel
*/

#include <stdbool.h>
#include <stdlib.h>

typedef struct arraylist {
    size_t capacity; // The size of the data array
    size_t stride; // Size of each entry in the arraylist
    size_t size; // How many entries are in the list
    __int8 data[]; // Pointer to the actual array list of data
} Arraylist;

struct arraylist* Arraylist_Create(size_t initSize, size_t typeSize);
void Arraylist_Destroy(struct arraylist* list);

void Arraylist_Add(struct arraylist** listPtr, void* data);
void Arraylist_Remove(struct arraylist* list, size_t index);
void Arraylist_Clear(struct arraylist* list);
int Arraylist_IndexOf(struct arraylist* list, void* data);
void Arraylist_Put(struct arraylist* list, size_t index, void* data);
void* Arraylist_Get(struct arraylist* list, size_t index);
void* Arraylist_Pop(struct arraylist* list);
bool Arraylist_Contains(struct arraylist* list, void* data);
void Arraylist_AssertSize(struct arraylist** listPtr, size_t size);

#endif