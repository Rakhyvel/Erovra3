#ifndef SORTED_LIST
#define SORTED_LIST

struct sortedListElem {
    unsigned int priority;
    unsigned int data;
};

typedef struct sortedList {
    int size;
    int capacity;
    struct sortedListElem* list;
} SortedList;

SortedList SortedList_Create();
void SortedList_Add(SortedList* list, unsigned int priority, unsigned int data);
void SortedList_Remove(SortedList* list, unsigned int data);

#endif