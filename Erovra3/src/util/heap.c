#include "heap.h"
#include <stdlib.h>

void swap(Heap* heap, int a, int b)
{
    struct pair temp = heap->data[a];
    heap->data[a] = heap->data[b];
    heap->data[b] = temp;
}

// Function to return the index of the
// parent node of a given node
int parent(int i)
{
    return (i - 1) / 2;
}

// Function to return the index of the
// left child of the given node
int leftChild(int i)
{
    return ((2 * i) + 1);
}

// Function to return the index of the
// right child of the given node
int rightChild(int i)
{
    return ((2 * i) + 2);
}

// Function to shift up the node in order
// to maintain the heap property
void shiftUp(Heap* heap, int i)
{
    while (i > 0 && heap->data[parent(i)].priority > heap->data[i].priority) {
        // Swap parent and current node
        swap(heap, parent(i), i);

        // Update i to parent of i
        i = parent(i);
    }
}

// Function to shift down the node in
// order to maintain the heap property
void shiftDown(Heap* heap, int i)
{
    int maxIndex = i;

    // Left Child
    int l = leftChild(i);

    if (l <= heap->size && heap->data[l].priority < heap->data[maxIndex].priority) {
        maxIndex = l;
    }

    // Right Child
    int r = rightChild(i);

    if (r <= heap->size && heap->data[r].priority < heap->data[maxIndex].priority) {
        maxIndex = r;
    }

    // If i not same as maxIndex
    if (i != maxIndex) {
        swap(heap, i, maxIndex);
        shiftDown(heap, maxIndex);
    }
}

// Function to insert a new element
// in the Binary Heap
void Heap_Insert(Heap* heap, int p, Uint32 d)
{
    heap->size++;
    heap->data[heap->size] = (struct pair) { p, d };

    // Shift Up to maintain heap property
    shiftUp(heap, heap->size);
}

// Function to extract the element with
// minimum priority
int extractMin(Heap* heap)
{
    int result = heap->data[0].data;

    // Replace the value at the root
    // with the last leaf
    heap->data[0] = heap->data[heap->size];
    heap->size--;

    // Shift down the replaced element
    // to maintain the heap property
    shiftDown(heap, 0);
    return result;
}

// Function to change the priority
// of an element
void Heap_ChangePriority(Heap* heap, int i, int p)
{
    int oldp = heap->data[i].priority;
    heap->data[i].priority = p;

    if (p < oldp) {
        shiftUp(heap, i);
    } else {
        shiftDown(heap, i);
    }
}

// Function to get value of the current
// maximum element
int Heap_GetMin(Heap* heap)
{
    return heap->data[0].data;
}

// Function to remove the element
// located at given index
void Heap_Remove(Heap* heap, int i)
{
    heap->data[i] = (struct pair) { heap->data[0].priority - 1, heap->data[0].data };

    // Shift the node to the root
    // of the heap
    shiftUp(heap, i);

    // Extract the node
    extractMin(heap);
}

int Heap_GetData(Heap* heap, int i)
{
    return heap->data[i].data;
}

Heap* Heap_Create(int size)
{
    Heap* retval = calloc(1, sizeof(Heap) + sizeof(struct pair) * size);
    retval->size = -1;
    return retval;
}

void Heap_Destroy(Heap* heap)
{
    free(heap);
}