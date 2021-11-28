#include "arraylist.h"
#include "./debug.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

void Arraylist_Destroy(Arraylist* list)
{
    if (list == NULL) {
        printf("WARNING: list was NULL!");
        return;
    }
    free(list);
}

bool Arraylist_Add(Arraylist** listPtr, void* data)
{
    if (*listPtr == NULL) {
        PANIC("List is NULL");
    } else if (data == NULL) {
        PANIC("Data is NULL");
    }

    bool retval = Arraylist_AssertSize(listPtr, (*listPtr)->size + 1);
    Arraylist_Put(*listPtr, (*listPtr)->size - 1, data);
    return retval;
}

void Arraylist_Remove(Arraylist* list, size_t index)
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

void Arraylist_Clear(Arraylist* list)
{
    list->size = 0;
}

int Arraylist_IndexOf(Arraylist* list, void* data)
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

void Arraylist_Put(Arraylist* list, size_t index, void* data)
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

void* Arraylist_Get(Arraylist* list, size_t index)
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
* Doesn't reduce memory pool, since the pointer will technically be "outside" the list.
*/
void* Arraylist_Pop(Arraylist* list)
{
    if (list == NULL) {
        PANIC("List is NULL");
    } else if (list->size == 0) {
        PANIC("List is too small to be popped");
    }

    list->size--;
    return list->data + list->size * list->stride;
}

bool Arraylist_Contains(Arraylist* list, void* data)
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

bool Arraylist_AssertSize(Arraylist** listPtr, size_t size)
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
                return true;
            } else {
                PANIC("Mem err, asked to reallocate %d", 2 * (*listPtr)->size * (*listPtr)->stride);
            }
        }
    }
    return false;
}