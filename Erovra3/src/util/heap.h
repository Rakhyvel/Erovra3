/*	heap.h
* 
*	A heap stores pairs of elements and their priority. This is a min heap, and
*	so it is very easy to get the element with the lowest priority (or cost).
* 
*	TODO: - Flexible sizes (use base arraylist)
*		  - Arbitrary length element sizes
* 
*	@author	Joseph Shimel
*	@date	9/12/21
*/

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

/**
 * @brief Creates a heap
 * @param size The number of elements in the heap. Size does not change!
 * @return Pointer to heap
*/
Heap* Heap_Create(int size);

/**
 * @brief Destroys a heap
 * @param heap Heap to destroy
*/
void Heap_Destroy(Heap* heap);

/**
 * @brief Inserts a new element into the heap. Heap is in sorted order after insert.
 * @param heap Heap to insert element into
 * @param p Priority of the data
 * @param d Data to insert to heap
*/
void Heap_Insert(Heap* heap, int p, unsigned int d);

/**
 * @brief Changes the priority of an element in the heap
 * @param heap The heap
 * @param i Index of the element in the heap
 * @param p New priority for element
*/
void Heap_ChangePriority(Heap* heap, int i, int p);

/**
 * @brief Finds and returns the element with the minimum priority in the heap. Does not remove element.
 * @param heap The heap
 * @return The data of the element with the minimum priority
*/
int Heap_GetMin(Heap* heap);

/**
 * @brief Removes an element from the heap at a given index
 * @param heap The heap
 * @param i Index of element to remove
*/
void Heap_Remove(Heap* heap, int i);

/**
 * @brief Returns the data of the element in the heap at a given index
 * @param heap The heap
 * @param i The index
 * @return The data of the element in the heap at the given index
*/
int Heap_GetData(Heap* heap, int i);

#endif