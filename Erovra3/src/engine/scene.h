/*	scene.h

	Date: 4/2/21
	Author: Joseph Shimel
*/
#pragma once
#include "../util/arraylist.h"
#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>

typedef Uint32 ComponentKey;
typedef Uint64 ComponentMask;
typedef Uint16 EntityIndex;
typedef Uint16 EntityVersion;
typedef Uint32 EntityID; // EntitiyIndex << 16 | EntityVersion

#define MAX_COMPONENTS 64
#define INVALID_ENTITY_INDEX ((EntityID)65535)
#define MAX_ENTITIES 1000

#define GET_COMPONENT_FIELD(scene, id, componentid, structName, fieldName) ((structName*)Scene_GetComponent(scene, id, componentid))->fieldName
#define SET_COMPONENT_FIELD(scene, id, componentid, structName, fieldName, value) (((structName*)Scene_GetComponent(scene, id, componentid))->fieldName) = value

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a##b

#define UNIQUE_NAME(base) CONCAT(base, __COUNTER__)

#define NUMARGS(...) (sizeof((int[]) { __VA_ARGS__ }) / sizeof(int))

#define system(scene, id, ...)                                                                \
    const ComponentMask CONCAT(mask, __LINE__) = Scene_CreateMask(scene, NUMARGS(__VA_ARGS__), __VA_ARGS__); \
    for (EntityID id = Scene_Begin(scene, CONCAT(mask, __LINE__)); Scene_End(scene, id); id = Scene_Next(scene, id, CONCAT(mask, __LINE__)))

#define system_mask(scene, id, mask) \
    for (EntityID id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask))

#define Scene_EntityHasComponents(scene, id, ...) Scene_EntityHasComponentMask(scene, Scene_CreateMask(scene, NUMARGS(__VA_ARGS__), __VA_ARGS__), id)

/*
	Stored in the "entities" arraylist */
typedef struct entity {
    EntityID id;
    ComponentMask mask;
} Entity;

typedef struct scene {
    /* Used to map random, global component keys to their actual index in this particular scene */
    ComponentKey lookupTable[MAX_COMPONENTS];
    /* Array of Arraylists, representing the actual data in the scene */
    struct arraylist* /*<Arraylist>*/ (components[MAX_COMPONENTS]);
    /* List of all entities in scene */
    struct arraylist* /*<Entity>*/ entities;
    /* List of purged entity indices To be cleared removed later */
    struct arraylist* /*<EntityIndex>*/ purgedEntities;
    /* List of free indices that can be overwritten */
    struct arraylist* /*<EntityIndex>*/ freeIndices;
    /* Number of entities in scene */
    int numEntities;
    /* Number of components in scene */
    int numComponents;
    const void (*update)(struct scene*);
    const void (*render)(struct scene*);
    const void (*destructor)(struct scene*);
    bool valid;
} Scene;

/*
	Initializes the components table, entities list, and purged list */
struct scene* Scene_Create(void(initComponents)(struct scene*), void (*update)(struct scene*), void (*render)(struct scene*), void (*destructor)(struct scene*));

void Scene_Destroy(struct scene* scene);

/* 
	Sets the component size for a componentID */
void Scene_RegisterComponent(struct scene* scene, const ComponentKey globalKey, size_t componentSize);
/*
	Returns a pointer to the begining of an entities component*/
void* Scene_GetComponent(struct scene* scene, EntityID id, ComponentKey globalKey);

/*
	Allocates a new entity, either resurrecting a dead entitiy, or creating a new one */
EntityID Scene_NewEntity(struct scene*);
/*
	Assigns a component to an entity, changing its bitmask */
void Scene_Assign(struct scene* scene, EntityID id, ComponentKey globalKey, void* componentData);
/*
	Unassigns a component to an entity */
void Scene_Unassign(struct scene* scene, EntityID id, ComponentKey globalKey);
/*
	Marks an entity for deletion the next update */
void Scene_MarkPurged(struct scene*, EntityID id);
void Scene_Purge(struct scene*);

const ComponentMask Scene_CreateMask(struct scene* scene, int number, ComponentKey components, ...);
bool Scene_EntityHasComponentMask(struct scene* scene, const ComponentMask mask, EntityID id);
bool Scene_EntityHasAnyComponents(struct scene* scene, const ComponentMask mask, EntityID id);

EntityID Scene_Begin(struct scene* scene, const ComponentMask mask);
bool Scene_End(struct scene* scene, EntityID id);
EntityID Scene_Next(struct scene* scene, EntityID prev, const ComponentMask mask);