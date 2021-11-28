/*	arraylist.h
*	
*	An arraylist is a contiguous pool of memory that grows as more data is 
*	added to it. Memory is copied into the arraylist, byte by byte.
* 
*	@author	Joseph Shimel
*	@date	4/2/21
*/

#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct arraylist {
    size_t capacity; // The size of the data array
    size_t stride; // Size of each element in the arraylist
    size_t size; // How many entries are in the list
    __int8 data[]; // Pointer to the actual array list of data
} Arraylist;

/**
 * @brief Creates an empty arraylist
 * @param initSize The initial capacity of the arraylist. This is how many free spaces the list starts with. 
 * @param typeSize The size, in bytes, of the items to be stored in the list
 * @return A pointer to the arraylist structure
*/
Arraylist* Arraylist_Create(size_t initSize, size_t typeSize);

/**
 * @brief Frees an arraylist. Fine to call on null list
 * @param list List to destroy
*/
void Arraylist_Destroy(Arraylist* list);

/**
 * @brief Copies data to end of list. Changes the pointer of the list in the case that it needs to grow.
 * @param listPtr Pointer to pointer of list. May be changed if list needs to grow.
 * @param data Pointer to begining of data. The number of bytes copied is the size of the type of the elements in the list.
 * @return Whether or not the list has been updated
*/
bool Arraylist_Add(Arraylist** listPtr, void* data);

/**
 * @brief Removes the element at an index in a list. List will keep the same order after removal.
 * @param list List to remove element from
 * @param index Index of element to remove
*/
void Arraylist_Remove(Arraylist* list, size_t index);

/**
 * @brief Removes all elements from a list
 * @param list List to clear
*/
void Arraylist_Clear(Arraylist* list);

/**
 * @brief Finds the first index of an element in a list. Returns -1 if element is not in list.
 * @param list List to search
 * @param data Pointer to data to match
 * @return The index of the first element in the list that matches the data, -1 if no match can be found.
*/
int Arraylist_IndexOf(Arraylist* list, void* data);

/**
 * @brief Copies data into a specific index of a list
 * @param list List to put data into
 * @param index Index to put data into
 * @param data Pointer to data to copy into list
*/
void Arraylist_Put(Arraylist* list, size_t index, void* data);

/**
 * @brief Returns a pointer to an element at a given index of a list
 * @param list List to get pointer of
 * @param index Element index
 * @return The pointer to the element at the given index
*/
void* Arraylist_Get(Arraylist* list, size_t index);

/**
 * @brief Removes and returns a pointer to the last element in the list. Pointer is valid until list is added to.
 * @param list The list to pop
 * @return The pointer to the last element in the list
*/
void* Arraylist_Pop(Arraylist* list);

/**
 * @brief Checks to see if a list contains a piece of data
 * @param list The list to check
 * @param data A pointer data to compare
 * @return Whether or not the list contains the specified data
*/
bool Arraylist_Contains(Arraylist* list, void* data);

/**
 * @brief Sets the size of the list. Resizes if the list is too small, extra space is garbage. Does nothing if list is bigger than given size
 * @param listPtr Pointer to pointer of list. List pointer will be updated if list is resized
 * @param size New size of list
 * @return Whether or not the list pointer has been updated
*/
bool Arraylist_AssertSize(Arraylist** listPtr, size_t size);

#endif