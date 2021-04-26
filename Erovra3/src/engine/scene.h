/*	scene.h

	Date: 4/2/21
	Author: Joseph Shimel
*/
#pragma once
#include "../util/arraylist.h"
#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>

typedef Uint8 ComponentID;
typedef Uint64 ComponentMask;
typedef Uint16 EntityIndex;
typedef Uint16 EntityVersion;
typedef Uint32 EntityID; // EntitiyIndex << 16 | EntityVersion

#define MAX_COMPONENTS sizeof(ComponentMask) * 8
#define INVALID_ENTITY_INDEX ((EntityID)65535)
#define MAX_ENTITIES 1000

#define GET_COMPONENT_FIELD(scene, id, componentid, structName, fieldName) ((structName*)Scene_GetComponent(scene, id, componentid))->fieldName
#define SET_COMPONENT_FIELD(scene, id, componentid, structName, fieldName, value) (((structName*)Scene_GetComponent(scene, id, componentid))->fieldName) = value

/*
	Stored in the "entities" arraylist */
typedef struct entity {
    EntityID id;
    ComponentMask mask;
} Entity;

typedef struct scene {
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
    struct arraylist* components[MAX_COMPONENTS];
    /* List of all entities in scene */
    struct arraylist* entities;
    /* List of purged entity indices To be cleared removed later */
    struct arraylist* purgedEntities;
    /* List of free indices that can be overwritten */
    struct arraylist* freeIndices;
    /* Number of entities in scene */
    int numEntities;
    /* Number of components in scene */
    int numComponents;
    const void (*update)(struct scene*);
    const void (*render)(struct scene*);
} Scene;

/*
	Initializes the components table, entities list, and purged list */
struct scene* Scene_Create(void(initComponents)(struct Scene*), void (*update)(struct Scene*), void (*render)(struct Scene*));

/* 
	Sets the component size for a componentID */
const ComponentID Scene_RegisterComponent(struct scene*, size_t);
/*
	Returns a pointer to the begining of an entities component*/
void* Scene_GetComponent(struct scene*, EntityID, ComponentID);

/*
	Allocates a new entity, either resurrecting a dead entitiy, or creating a new one */
EntityID Scene_NewEntity(struct scene*);
/*
	Assigns a component to an entity, changing its bitmask */
void Scene_Assign(struct scene*, EntityID, ComponentID, void* componentData);
/*
	Marks an entity for deletion the next update */
void Scene_MarkPurged(struct scene*, EntityID id);
void Scene_Purge(struct scene*);

ComponentMask Scene_CreateMask(int, ComponentID, ...);
bool Scene_EntityHasComponent(struct scene* scene, ComponentMask mask, EntityID id);

EntityID Scene_Begin(struct scene* scene, ComponentMask mask);
bool Scene_End(struct scene* scene, EntityID id);
EntityID Scene_Next(struct scene* scene, EntityID prev, ComponentMask mask);