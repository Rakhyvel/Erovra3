/*	scene.h
* 
*	A simple ECS framework for the application. Entities are an index to a mask table,
*	and all component tables. Component tables are arraylist memory pools of
*	component data.
* 
*	Special thanks to David Colson for his excellent blog post on the subject
*	https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html
* 
*	# COMPONENTKEY TO COMPONENTID MAPPING
*	Suppose we wanted to import two libraries that both defined their own 
*	components. Both libraries would need to have specific compile time values
*	to reference each component, and also have to allocate components in a way
*	to not clash with each other.
* 
*	The way this is resolved is that each component has a ComponentKey, which is
*	a random 32-bit integer, which is converted to a ComponentID for each scene
*	during run time.
* 
*	@author Joseph Shimel
*	@date	4/2/21
*/

#ifndef SCENE_H
#define SCENE_H

#include "../util/arraylist.h"
#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_COMPONENTS 64
#define INVALID_ENTITY_INDEX ((EntityID)65535)
#define INVALID_ENTITY_ID (INVALID_ENTITY_INDEX << 16);
#define MAX_ENTITIES 1000

#define GET_COMPONENT_FIELD(scene, id, componentid, structName, fieldName) ((structName*)Scene_GetComponent(scene, id, componentid))->fieldName
#define SET_COMPONENT_FIELD(scene, id, componentid, structName, fieldName, value) (((structName*)Scene_GetComponent(scene, id, componentid))->fieldName) = value

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a##b

/* Calculates the number of arguments in a vararg list */
#define NUMARGS(...) (sizeof((int[]) { __VA_ARGS__ }) / sizeof(int))

/* Used to iterate through the entities in a scene that have all of a varargs list of ComponentKeys */
#define system(scene, id, ...)                                                                \
    const ComponentMask CONCAT(mask, __LINE__) = Scene_CreateMask(scene, NUMARGS(__VA_ARGS__), __VA_ARGS__); \
    for (EntityID id = Scene_Begin(scene, CONCAT(mask, __LINE__)); Scene_End(scene, id); id = Scene_Next(scene, id, CONCAT(mask, __LINE__)))

/* Used to iterate through the entities in a scene that match a ComponentMask */
#define system_mask(scene, id, mask) \
    for (EntityID id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask))

/* Checks to see if an entity has all the components of the given varargs list */
#define Scene_EntityHasComponents(scene, id, ...) Scene_EntityHasComponentMask(scene, Scene_CreateMask(scene, NUMARGS(__VA_ARGS__), __VA_ARGS__), id)

/* A random, universal key that corresponds to a component. Is mapped in each scene to a specific ComponentID */
typedef Uint64 ComponentKey;
/* Bitmask of ComponentIDs that can be used to specify a collection of components */
typedef Uint64 ComponentMask;
/* The index of an entity in a scene's component and bitmask arrays */
typedef Uint16 EntityIndex;
/* The version of each index, used to prevent stale entities */
typedef Uint16 EntityVersion;
/* A UID for an entity, EntitiyIndex << 16 | EntityVersion */
typedef Uint32 EntityID;

/* An entity has an ID, and a component mask. The ID contains an index and a version. */
typedef struct entity {
    EntityID id;
    ComponentMask mask;
} Entity;

/**
 * @brief Scenes have a collection of entities and information relating to
 * those entities.
*/
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
    /* Called 60 times per second to update the state of the scene */
    const void (*update)(struct scene*);
    /* Called often to render the scene to the screen */
    const void (*render)(struct scene*);
    /* Destroys a scene and its dynamic memory */
    const void (*destructor)(struct scene*);
    /* Whether or not the scene has been destroyed and is now invalid */
    bool valid;
} Scene;

/*
	Initializes the components table, entities list, and purged list */

/**
 * @brief Initializes the scene
 * @param registerComponents	A callback used to register all ComponentKeys with a free ComponentID in a scene
 * @param update	A callback for the update function of a scene that is called by the game loop 60 times a second
 * @param render	A callback for the render function of a scene that is called by the game loop often
 * @param destructor	A callback for the destructor of a function which frees all allocated memory
 * @return	The created scene
*/
struct scene* Scene_Create(void(registerComponents)(struct scene*), void (*update)(struct scene*), void (*render)(struct scene*), void (*destructor)(struct scene*));

/**
 * @brief Destroys a scene. Calls the scene destructor. Invalidates scene. Should only be called by engine after popping a scene off the scene stack
 * @param scene Scene to destroy
*/
void Scene_Destroy(struct scene* scene);

/**
 * @brief Maps a ComponentKey to a free ComponentID for this particular scene. Allocates the memory pool for the component.
 * @param scene Scene to register the component with
 * @param globalKey A random, unique 32-bit integer
 * @param componentSize Size in bytes of the component data
*/
void Scene_RegisterComponent(struct scene* scene, const ComponentKey globalKey, size_t componentSize);

/**
 * @brief Returns a pointer to the component's data in the scene's memory pool
 * @param scene Scene to get the data from
 * @param id ID of the entity. Must be a recent version
 * @param globalKey ComponentKey of the component to retrieve data from
 * @return The pointer to the component data
*/
void* Scene_GetComponent(struct scene* scene, EntityID id, ComponentKey globalKey);

/**
 * @brief Allocates a new entity. New entities have a unique EntityID, and a clear component mask.
 * @param scene Scene to allocate the entity to
 * @return The EntityID of the allocated entity
*/
EntityID Scene_NewEntity(struct scene* scene);

/**
 * @brief Assigns a component to an entity. Double assignment is ok. NULL ComponentData acts as a flag.
 * @param scene Relevant scene
 * @param id Entity to assignt the component to
 * @param globalKey Component to assign
 * @param componentData Pointer to data to copy for entity. If NULL, will not be copied. 
*/
void Scene_Assign(struct scene* scene, EntityID id, ComponentKey globalKey, void* componentData);

/**
 * @brief Unassigns a component from an entity, or does nothing if component is not assigned to entity.
 * @param scene Relevant scene
 * @param id Entity to unassign from
 * @param globalKey Component to unassign from an entity
*/
void Scene_Unassign(struct scene* scene, EntityID id, ComponentKey globalKey);

/**
 * @brief Marks an entity for purge the after this tick has finished. Will remain valid for the remainder of the tick
 * @param scene	Relevent scene
 * @param id Entity to purge
*/
void Scene_MarkPurged(struct scene* scene, EntityID id);

/**
 * @brief Purges all entities marked for purge. Called by game loop after update, if scene stack is not stale.
 * @param scene Relevant scene
*/
void Scene_Purge(struct scene* scene);

bool Scene_EntityIsValid(struct scene* scene, EntityID id);

/**
 * @brief Takes a list of ComponentKeys and forms a ComponentMask for the relevant scene.
 * @param scene Scene that contains the ComponentKey to ComponentID mapping
 * @param number Number of ComponentKeys to read in
 * @param components ComponentKey varargs list
 * @return The created ComponentMask
*/
const ComponentMask Scene_CreateMask(struct scene* scene, int number, ComponentKey components, ...);

/**
 * @brief Checks whether or not the entity mask of an entity matches a component mask
 * @param scene Relevent scene
 * @param mask ComponentMask to check
 * @param id EntityID to check
 * @return Whether or not the entity's component mask matches the given component mask
*/
bool Scene_EntityHasComponentMask(struct scene* scene, const ComponentMask mask, EntityID id);

/**
 * @brief Checks to see if an entity has any of the components in a component mask
 * @param scene Relevant scene
 * @param mask Mask of components to check
 * @param id Entity to check
 * @return Whether or not the entity has any of the given components
*/
bool Scene_EntityHasAnyComponents(struct scene* scene, const ComponentMask mask, EntityID id);

/**
 * @brief Finds the first entity in the scene's list of entities that matches the given component mask
 * @param scene The scene to iterate through
 * @param mask Component mask to match
 * @return The first entity in the scene that matches the component mask
*/
EntityID Scene_Begin(struct scene* scene, const ComponentMask mask);

/**
 * @brief Determines if the given entity is the last entity in the scene's list of entities
 * @param scene The relevant scene
 * @param id The entity to check
 * @return Whether or not the entity is the last entity in the scene
*/
bool Scene_End(struct scene* scene, EntityID id);

/**
 * @brief Finds an entity after a given entity that matches a component mask
 * @param scene Scene to iterate through
 * @param prev A previous entity in the scene's list of entities
 * @param mask A component mask to match
 * @return The next entity in the scene's list of entities
*/
EntityID Scene_Next(struct scene* scene, EntityID prev, const ComponentMask mask);

#endif