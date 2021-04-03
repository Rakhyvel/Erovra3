#pragma once
struct arraylist {
	char* data;
	int typeSize;
	int size;
	int capacity;
};

struct arraylist* arraylist_create(int typeSize);

void arraylist_add(struct arraylist* list, void* data);
void arraylist_put(struct arraylist* list, int index, void* data);
void* arraylist_get(struct arraylist* list, int index);