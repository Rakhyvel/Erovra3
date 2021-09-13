#ifndef HEAP_H
#define HEAP_H

#include <SDL.h>

struct pair {
    int priority;
    Uint32 data;
};

typedef struct heap {
    int size;
    struct pair data[1];
} Heap;

Heap* Heap_Create(int size);
void Heap_Destroy(Heap* heap);

void Heap_Insert(Heap* heap, int p, Uint32 d);
void Heap_ChangePriority(Heap* heap, int i, int p);
int Heap_GetMin(Heap* heap);
void Heap_Remove(Heap* heap, int i);
int Heap_GetData(Heap* heap, int i);

#endif