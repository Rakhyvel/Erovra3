#include "sortedlist.h"
#include "./debug.h"
#include <stdlib.h>

SortedList SortedList_Create()
{
    SortedList retval;
    retval.size = 0;
    retval.capacity = 10;
    retval.list = malloc(sizeof(struct sortedListElem) * retval.capacity);
    return retval;
}

void SortedList_Add(SortedList* list, unsigned int priority, unsigned int data)
{
    // Iterate until priority fits
    int i;
    for (i = 0; i < list->size; i++) {
        struct sortedListElem elem = list->list[i];
        if (elem.priority > priority) {
            break;
        }
    }
    // i will now be the index of the next greatest priority, or list->size
    if (i >= list->capacity) {
        list->capacity *= 2;
        struct sortedListElem* newList = realloc(list->list, sizeof(struct sortedListElem) * list->capacity);
        if (!newList) {
            PANIC("Mem error\n");
        } else {
            list->list = newList;
        }
    }

    // Work backwards, shift every element over one
    for (int j = list->size; j > i; j--) {
        list->list[j] = list->list[j - 1];
    }

    // Finally insert element
    list->list[i] = (struct sortedListElem) { priority, data };
    list->size++;
}

void SortedList_Remove(SortedList* list, unsigned int data)
{
    int i;
    for (i = 0; i < list->size; i++) {
        struct sortedListElem elem = list->list[i];
        if (elem.data == data) {
            break;
        }
    }
    for (; i < list->size - 1; i++) {
        list->list[i] = list->list[i + 1];
    }
    list->size--;
}