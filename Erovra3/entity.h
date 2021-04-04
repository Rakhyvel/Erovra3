/*	entity.h

	Date: 4/2/21
	Author: Joseph Shimel
*/
#pragma once
#include <SDL.h>
#include <stdlib.h>
#include "arraylist.h"

typedef Uint8 ComponentID;
typedef Uint64 ComponentMask;
typedef Uint32 EntityIndex;
typedef Uint32 EntityVersion;
typedef Uint64 EntityID; // EntitiyIndex << 32 | EntityVersion

#define MAX_COMPS sizeof(ComponentMask) * 8
#define INVALID_ENTITY_INDEX ((EntityID)-1)

#define GET_COMPONENT(id, componentid, type) ((type*)ECS_GetComponent(id, componentid))

/*
	Stored in the "entities" arraylist */
struct entity {
	EntityID id;
	ComponentMask mask;
};

/* 
	Jagged 2D array of component data
	Components ->
	|---|---|---| E
	| 0 | 0 | 0 | n
	| x |---| x | t
	| x | 1 |---| i
	|---|---| 1 | t
	| 1 |   | x | i
	| x |   |---| e
	| x |   |   | s
	|---|   |   | 
	|   |   |   |
*/
struct arraylist* components[MAX_COMPS];
/* List of all entities in scene */
struct arraylist* entities;
/* List of purged entity indices To be cleared removed later */
struct arraylist* purgedEntities;
/* List of free indices that can be overwritten */
struct arraylist* freeIndices;
/* Number of entities in existance */
int numEntities;


/*
	Initializes the components table, entities list, and purged list */
void ECS_Init();

/* 
	Sets the component size for a componentID */
void ECS_RegisterComponent(ComponentID, size_t);
/*
	Returns a pointer to the begining of an entities component*/
void* ECS_GetComponent(EntityID, ComponentID);

/*
	Allocates a new entity, either resurrecting a dead entitiy, or creating a new one */
EntityID ECS_NewID();
/*
	Assigns a component to an entity, changing its bitmask */
void ECS_Assign(EntityID, ComponentID, void* componentData);
/*
	Marks an entity for deletion the next update */
void ECS_MarkPurged(EntityID id);
void ECS_Purge();