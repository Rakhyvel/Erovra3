/*	scene.c

	A simple ECS framework for the game. Entities are an index to a mask table,
	and all component tables. Component tables are arraylist memory pools of
	component data.

	Special thanks to David Colson for his excellent blog post on the subject
	https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html

	Date: 4/2/21
	Author: Joseph Shimel
*/

#pragma once
#include "scene.h"
#include "../util/debug.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef Uint8 ComponentID;
#define INVALID_COMPONENT_ID 0

static EntityIndex getIndex(EntityID);
static EntityVersion getVersion(EntityID);
static struct entity* getEntityStruct(struct scene*, EntityID);
ComponentID getComponentID(struct scene* scene, ComponentKey globalKey);

/*
	Initializes the memory pools for a scene */
struct scene* Scene_Create(void(initComponents)(struct scene*), void (*update)(struct scene*), void (*render)(struct scene*), void (*destructor)(struct scene*))
{
    struct scene* retval = calloc(1, sizeof(struct scene));
    if (!retval) {
        PANIC("Memory error");
    }

    retval->entities = Arraylist_Create(10, sizeof(struct entity));
    retval->purgedEntities = Arraylist_Create(10, sizeof(EntityIndex));
    retval->freeIndices = Arraylist_Create(10, sizeof(EntityIndex));
    retval->update = update;
    retval->render = render;
    retval->destructor = destructor;
    retval->valid = true;

    for (int i = 0; i < MAX_COMPONENTS; i++) {
        retval->lookupTable[i] = INVALID_COMPONENT_ID;
        retval->components[i] = NULL;
    }

    initComponents(retval);

    return retval;
}

/*	Calls a scene's destructor, then frees the memory used by a scene.*/
void Scene_Destroy(struct scene* scene)
{
    if (scene == NULL) {
        printf("WARNING: scene was NULL!");
        return;
    }
    if (scene->destructor) {
        scene->destructor(scene);
    }
    scene->valid = false;
    for (int i = 0; i < scene->numComponents; i++) {
        if (scene->components[i] != NULL) {
            Arraylist_Destroy(scene->components[i]);
        }
    }
    Arraylist_Destroy(scene->entities);
    Arraylist_Destroy(scene->purgedEntities);
    Arraylist_Destroy(scene->freeIndices);
    free(scene);
}

/*	Takes in a global component key, and maps it to a free ComponentID for this particular scene. */
void Scene_RegisterComponent(struct scene* scene, ComponentKey globalKey, size_t componentSize)
{
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    }
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (scene->lookupTable[i] == INVALID_COMPONENT_ID) {
            scene->lookupTable[i] = globalKey;
            scene->components[i] = Arraylist_Create(MAX_ENTITIES, componentSize);
            scene->numComponents++;
            return;
        }
    }
    PANIC("All out of components!\n");
}

/*
	Returns a pointer to the component data for a given entity and a given component.
	
	EntityID must be valid, componentID must be valid, and entity must have component  */
void* Scene_GetComponent(struct scene* scene, EntityID id, ComponentKey globalKey)
{
    ComponentID componentID = getComponentID(scene, globalKey);
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    } else if (getIndex(id) == INVALID_ENTITY_INDEX) {
        PANIC("Invalid entity index");
    } else if (componentID >= MAX_COMPONENTS || componentID < 0) {
        PANIC("Invalid entity index");
    } else if (scene->components[componentID] == NULL) {
        PANIC("Component not registered yet");
    } else if (!Scene_EntityHasComponents(scene, id, globalKey)) {
        for (int i = 0; i < scene->numEntities; i++) {
            struct entity* entt = ARRAYLIST_GET(scene->entities, i, struct entity);
        }
        struct entity* entt = ARRAYLIST_GET(scene->entities, getIndex(id), struct entity);
        PANIC("Entity %d does not have component %d, mask is %d. Scene: %p", id >> 16, componentID, entt->mask, scene); // Sometimes entt is an invalid address and throws access violation, from BuyX(), from AIInfantryBuild(). Also from ProduceResources()
    } else if (getVersion(ARRAYLIST_GET(scene->entities, getIndex(id), struct entity)->id) != getVersion(id)) {
        PANIC("Outdated EntityID. Version is %d, given version was %d", getVersion(ARRAYLIST_GET(scene->entities, getIndex(id), struct entity)->id), getVersion(id));
    } else {
        return Arraylist_Get(scene->components[componentID], getIndex(id));
    }
}

/*
	Allocates and returns a new EntityID. Will either reallocate a freed 
	reference, or create a new one.
	
	Entities are gauranteed to have a unique index-version number pairing
	than all other entities created before.
	
	Entities are gauranteed to have a clear component mask. */
EntityID Scene_NewEntity(struct scene* scene)
{
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    } else if (scene->entities->size > MAX_ENTITIES) {
        PANIC("Entity overflow");
    }

    if (scene->freeIndices->size != 0) {
        EntityIndex index = ARRAYLIST_POP_DEREF(scene->freeIndices, EntityIndex);
        struct entity* entity = ARRAYLIST_GET(scene->entities, index, struct entity);
        entity->id = ((EntityID)index << 16) | getVersion(entity->id) + 1;
        entity->mask = 0;
        scene->numEntities++;
        return entity->id;
    } else {
        EntityIndex index = (EntityIndex)(scene->entities->size);
        struct entity newEntity = { ((EntityID)index << 16 | 1), 0 };
        Arraylist_Add(scene->entities, &newEntity);
        scene->numEntities++;
        for (int i = 0; i < scene->numComponents; i++) {
            Arraylist_AssertSize(scene->components[i], 2 * index + 1);
        }
        return newEntity.id;
    }
}

/*
	Assigns a component to an entity. Double assignment is ok.

	If given componentData argument is NULL, only the mask will be changed.
	ComponentID does not need to be registered. Component can then act as a flag.
	
	If given componentData argument is not NULL, component must be registered.
	Data will then be copied from dereference of pointer to component data
	memory pool. */
void Scene_Assign(struct scene* scene, EntityID id, ComponentKey globalKey, void* componentData)
{
    ComponentID componentID = getComponentID(scene, globalKey);
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    } else if (getIndex(id) == INVALID_ENTITY_INDEX || getIndex(id) > MAX_ENTITIES) {
        PANIC("Invalid entity index");
    } else if (componentID >= MAX_COMPONENTS || componentID < 0) {
        PANIC("Invalid entity index");
    } else if (componentData == NULL) {
        getEntityStruct(scene, id)->mask |= ((ComponentMask)1 << componentID);
    } else if (scene->components[componentID] == NULL) {
        PANIC("Component not registered yet %d", globalKey);
    } else {
        getEntityStruct(scene, id)->mask |= ((ComponentMask)1 << componentID);
        Arraylist_Put(scene->components[componentID], getIndex(id), componentData);
    }
}

/*
	Takes in an entity ID, and a componentID. Unassigns the component from the
	entity's component mask
	
	Sequential unassigns of the same entity and component id are allowed. This
	means you can unassign a component to an entity that does not have the 
	component assigned. (Might change?) */
void Scene_Unassign(struct scene* scene, EntityID id, ComponentKey globalKey)
{
    ComponentID componentID = getComponentID(scene, globalKey);
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    } else if (getIndex(id) == INVALID_ENTITY_INDEX || getIndex(id) > MAX_ENTITIES) {
        PANIC("Invalid entity index");
    } else if (componentID >= MAX_COMPONENTS || componentID < 0) {
        PANIC("Invalid entity index");
    } else if (scene->components[componentID] == NULL) {
        PANIC("Component not registered yet");
    } else {
        getEntityStruct(scene, id)->mask &= ~((ComponentMask)1 << componentID);
    }
}

/*
	Marks an entity as purged, and ready for deallocation when the purge 
	function is called 
	
	The same entity can be marked purged more than once. Any call after the first 
	call, though, has no effect. */
void Scene_MarkPurged(struct scene* scene, EntityID id)
{
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    }
    int index = getIndex(id);
    Arraylist_Add(scene->purgedEntities, &index);
}

/*
	Frees all purged entities.
	
	Marks index as invalid, increments version, clears mask, and finally frees
	the index in the entity thing. */
void Scene_Purge(struct scene* scene)
{
    while (scene->purgedEntities->size > 0) {
        EntityIndex index = ARRAYLIST_POP_DEREF(scene->purgedEntities, EntityIndex);
        struct entity* purgedEntity = ARRAYLIST_GET(scene->entities, index, struct entity);
        purgedEntity->id = (INVALID_ENTITY_INDEX << 16) | getVersion(purgedEntity->id);
        purgedEntity->mask = 0;
        scene->numEntities--;
        // must gaurd against duplicates, otherwise allocated entity would be marked "free"
        if (!Arraylist_Contains(scene->freeIndices, &index)) {
            Arraylist_Add(scene->freeIndices, &index);
        }
    }
}

/*
	Creates a component bit mask based on given a list of component ids */
const ComponentMask Scene_CreateMask(struct scene* scene, int number, ComponentKey components, ...)
{
    ComponentKey component = components;
    ComponentMask retval = 0;
    va_list args;
    va_start(args, components);

    for (int i = 0; i < number; i++) {
        retval |= ((ComponentMask)1 << (ComponentMask)getComponentID(scene, component));
        component = va_arg(args, ComponentKey);
    }

    va_end(args);

    return (const ComponentMask)retval;
}

/*
	Returns whether or not the entity matches a component mask */
bool Scene_EntityHasComponentMask(struct scene* scene, const ComponentMask mask, EntityID id)
{
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    }
    EntityIndex index = getIndex(id);
    if (index > scene->entities->size) {
        PANIC("Malformed EntityID (i: %d | v: %d)", getIndex(id), getVersion(id));
    } else if (getVersion(ARRAYLIST_GET(scene->entities, getIndex(id), struct entity)->id) != getVersion(id)) {
        PANIC("Outdated EntityID. Version is %d, given version was %d", getVersion(ARRAYLIST_GET(scene->entities, getIndex(id), struct entity)->id), getVersion(id));
    }
    struct entity* entt = ARRAYLIST_GET(scene->entities, index, struct entity);
    return (entt->mask & mask) == mask;
}

/*
	Returns whether or not the entity mathces some of the components in a mask */
bool Scene_EntityHasAnyComponents(struct scene* scene, const ComponentMask mask, EntityID id)
{
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    }
    EntityIndex index = getIndex(id);
    struct entity* entt = ARRAYLIST_GET(scene->entities, index, struct entity);
    return (entt->mask & mask) != 0;
}

/*
	Finds and returns first index of a mask matching an entity */
EntityID Scene_Begin(struct scene* scene, const ComponentMask mask)
{
    return Scene_Next(scene, -1, mask);
}

/*
	Checks to see if the entity id is the last entity */
bool Scene_End(struct scene* scene, EntityID id)
{
    return getIndex(id) < scene->entities->size;
}

/*
	Returns the next entity id given a previous entity id, and a component mask */
EntityID Scene_Next(struct scene* scene, EntityID prev, const ComponentMask mask)
{
    EntityIndex index;
    int i = 0;
    for (index = getIndex(prev) + 1; index < scene->entities->size; index++) {
        struct entity* entt = ARRAYLIST_GET(scene->entities, index, struct entity);
        if (getIndex(entt->id) != INVALID_ENTITY_INDEX && (entt->mask & mask) == mask) {
            return entt->id;
        }
    }
    return (EntityID)(scene->entities->size) << 16;
}

// returns the index portion of an id
EntityIndex getIndex(EntityID id)
{
    return (id >> 16) & 0xFFFF;
}

// returns the version portion of an id
EntityVersion getVersion(EntityID id)
{
    return id & 0xFFFF;
}

// returns the entity structure for a given EntityID
struct entity* getEntityStruct(struct scene* scene, EntityID id)
{
    return ARRAYLIST_GET(scene->entities, getIndex(id), struct entity);
}

ComponentID getComponentID(struct scene* scene, ComponentKey globalKey)
{
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    }
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (scene->lookupTable[i] == globalKey) {
            return i;
        }
    }
    PANIC("Component key not registered %d\n", globalKey);
}