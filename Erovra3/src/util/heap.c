#include "heap.h"
#include <stdlib.h>

/**
 * @brief Swaps the data of two pairs in the heap
 * @param heap The relevant heap
 * @param a Index of first pair
 * @param b Index of second pair
*/
static void swap(Heap* heap, int a, int b)
{
    struct pair temp = heap->data[a];
    heap->data[a] = heap->data[b];
    heap->data[b] = temp;
}

/**
 * @brief Calculates the index of a parent node from a given node
 * @param i Child node
 * @return The index of the parent node
*/
static int parent(int i)
{
    return (i - 1) / 2;
}

/**
 * @brief Calculates the index of the left child of a given node
 * @param i Parent node
 * @return The index of the left child of a parent node
*/
static int leftChild(int i)
{
    return ((2 * i) + 1);
}

/**
 * @brief Calculates the index of the right child of a given node
 * @param i Parent node
 * @return The index of the right child of a parent node
*/
static int rightChild(int i)
{
    return ((2 * i) + 2);
}

/**
 * @brief Shifts up a node in the heap, based on the priority of the node. Used to maintain the heap's sorted property
 * @param heap The relevent heap
 * @param i Index of the node
*/
static void shiftUp(Heap* heap, int i)
{
    while (i > 0 && heap->data[parent(i)].priority > heap->data[i].priority) {
        // Swap parent and current node
        swap(heap, parent(i), i);

        // Update i to parent of i
        i = parent(i);
    }
}

/**
 * @brief Shifts down a node in the heap, based on the priority of the node. Used to maintain the heap's sorted property.
 * @param heap The relevant heap
 * @param i Index of the node
*/
static void shiftDown(Heap* heap, int i)
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

/**
 * @brief Finds the element with the minimum priority
 * @param heap Heap to find the min from
 * @return The data of the element with the least priority
*/
static int extractMin(Heap* heap)
{
    int result = heap->data[0].data;

    // Replace the value at the root with the last leaf
    heap->data[0] = heap->data[heap->size];
    heap->size--;

    // Shift down the replaced element to maintain the heap property
    shiftDown(heap, 0);
    return result;
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

void Heap_Insert(Heap* heap, int p, unsigned int d)
{
    heap->size++;
    heap->data[heap->size] = (struct pair) { p, d };

    // Shift Up to maintain heap property
    shiftUp(heap, heap->size);
}

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

int Heap_GetMin(Heap* heap)
{
    return heap->data[0].data;
}

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