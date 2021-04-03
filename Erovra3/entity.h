#pragma once
#include <stdlib.h>
#include "arraylist.h"

typedef unsigned long long EntityID;
#define MAX_COMPS sizeof(int) // because mask is an int

struct entity {
	EntityID id;
	int mask;
};

/* 
	Jagged 2D array of component data
	Components ->
	|---|---|---| E
	| x | x | x | n
	| x |---| x | t
	| x | x |---| i
	|---|---| x | t
	| x |   | x | i
	| x |   |---| e
	| x |   |   | s
	|---|   |   | 
	|   |   |   |
*/
struct arraylist* components[MAX_COMPS];
/* List of all entities in scene */
struct arraylist* entities;
/* The size of the entities list */
int numEntities;
/* List of purged entities. To be cleared removed later */
struct arraylist* purgedEntities;
/* List of free spaces that can be allocated to */
struct arraylist* freedEntities;


/*
	Initializes the components table, entities list, and purged list */
void ECS_Init();

/* 
	Sets the component size for a componentID */
void ECS_RegisterComponent(int componentID, size_t componentSize);
/*
	Returns a pointer to the begining of an entities component*/
void* ECS_GetComponent(EntityID id, int componentID);

/*
	Allocates a new entity, either resurrecting a dead entitiy, or creating a new one */
EntityID ECS_NewID();
/*
	Assigns a component to an entity, changing its bitmask */
void ECS_Assign(EntityID id, int componentID);
/*
	Marks an entity for deletion the next update */
void ECS_Purge(EntityID id);
