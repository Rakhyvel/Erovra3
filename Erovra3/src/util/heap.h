#ifndef HEAP_H
#define HEAP_H

struct pair {
    int priority;
    unsigned int data;
};

typedef struct heap {
    int size;
    struct pair data[1];
} Heap;

Heap* Heap_Create(int size);
void Heap_Destroy(Heap* heap);

void Heap_Insert(Heap* heap, int p, unsigned int d);
void Heap_ChangePriority(Heap* heap, int i, int p);
int Heap_GetMin(Heap* heap);
void Heap_Remove(Heap* heap, int i);
int Heap_GetData(Heap* heap, int i);

#endif