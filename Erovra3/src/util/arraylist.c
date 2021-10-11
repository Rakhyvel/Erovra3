/*	arraylist.c

	An arraylist is a pool of memory that grows as more memory is added to it.
	Acts like a regular list. Memory is copied byte by byte when added, memory
	address from within the list are returned.

	Arraylists have a stride, which is the number of bytes each element in
	the pool is.

	Date: 4/2/21
	Author: Joseph Shimel
*/

#include "arraylist.h"
#include "debug.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
	Creates an arraylist with a given type size */
struct arraylist* Arraylist_Create(size_t initCapacity, size_t stride)
{
    // Allocate space for header info + space for data array
    struct arraylist* retval = calloc(1, (size_t)(sizeof(struct arraylist) + stride * initCapacity));
    if (!retval) {
        PANIC("Memory error");
    }
    retval->stride = stride;
    retval->size = 0;
    retval->capacity = initCapacity;
    return retval;
}

void Arraylist_Destroy(struct arraylist* list)
{
    if (list == NULL) {
        printf("WARNING: list was NULL!");
        return;
    }
    free(list);
}

/*
	Returns a pointer to within the arraylist at a given index */
void* Arraylist_Get(struct arraylist* list, size_t index)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (index > list->size) {
        PANIC("List index out of bounds. List size is %d, index was %d", list->size, index);
    } else if (index < 0) {
        PANIC("Negative index. Index was %d", index);
    }
    return list->data + (index * list->stride);
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
    return list->data + list->size * list->stride;
}

/*
* Copies the the data pointed to by the given data pointer to the end of an 
* arraylist. Also functions as a "push" function for stacks and queues.
* 
* Note that this function MAY cause the pointer to the Arraylist to become 
* updated in the event that the list needs to be realloc'd to grow.
* 
* @param listPtr	A pointer to a pointer to a list. May be changed by this
*					function if the list needs to grow.
* @param data		A pointer to the data to copy. This function will copy and
*					add the stride length of the list of bytes.
*/
void Arraylist_Add(struct arraylist** listPtr, void* data)
{
    if (*listPtr == NULL) {
        PANIC("List is NULL");
    } else if (data == NULL) {
        PANIC("Data is NULL");
    }

    Arraylist_AssertSize(listPtr, (*listPtr)->size + 1);
    Arraylist_Put(*listPtr, (*listPtr)->size - 1, data);
}

/*	Removes an entry of a list at a given index. The list is gauranteed to keep
 *	the same order after this function is called.
 * 
 *	@param list		Pointer to list
 *	@param index	Index of entry to remove
*/
void Arraylist_Remove(struct arraylist* list, size_t index)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (index > list->size) {
        PANIC("Index out of bounds. Index was %d, list size is %d", index, list->size);
    } else if (index < 0) {
        PANIC("Negative index. Index was %d", index);
    }
    memcpy(list->data + index * list->stride, list->data + (index + 1) * list->stride, (list->size - index - 1) * list->stride);
    list->size--;
}

int Arraylist_IndexOf(struct arraylist* list, void* data)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (data == NULL) {
        PANIC("Data is NULL");
    }

    for (int i = 0; i < list->size; i++) {
        if (!memcmp(list->data + (list->stride * i), data, list->stride)) {
            return i;
        }
    }
    return -1;
}

/*
	Copies data to an index */
void Arraylist_Put(struct arraylist* list, size_t index, void* data)
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

    memcpy(list->data + index * list->stride, data, list->stride);
}

bool Arraylist_Contains(struct arraylist* list, void* data)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (data == NULL) {
        PANIC("Data is NULL");
    }

    for (int i = 0; i < list->size; i++) {
        if (!memcmp(list->data + (list->stride * i), data, list->stride)) {
            return true;
        }
    }
    return false;
}

/*	Sets the size of the arraylist. Resizes the arraylist if it is too small. 
 *	Does nothing if list type size (stride) is 0. Does nothing if list size is 
 *	already larger than given size.
 *
 *	@param listPtr	Pointer to a pointer to an Arraylist. Done this way so that I
 *					can change the pointer to the Arraylist if a realloc needs to
 *					be done.
 *  @param size		Size to assert.
 */
void Arraylist_AssertSize(struct arraylist** listPtr, size_t size)
{
    if (*listPtr == NULL) {
        PANIC("List is NULL");
    }
    // Exit if list is ethereal
    if ((*listPtr)->stride == 0) {
        return;
    }

    if (size > (*listPtr)->size) {
        (*listPtr)->size = size;
        if ((*listPtr)->size >= (*listPtr)->capacity) {
            (*listPtr)->capacity = (*listPtr)->size * 2;
            void* res = realloc(*listPtr, sizeof(struct arraylist) + (*listPtr)->stride * (*listPtr)->capacity);
            if (res != NULL) {
                *listPtr = res;
            } else {
                PANIC("Mem err, asked to reallocate %d", 2 * (*listPtr)->size * (*listPtr)->stride);
            }
        }
    }
}