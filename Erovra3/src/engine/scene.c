/*	scene.c
* 
*	@author	Joseph Shimel
*	@date	4/2/21
*/

#include "apricot.h"
#include "scene.h"
#include "../util/debug.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define INVALID_COMPONENT_ID 0

typedef Uint8 ComponentID;

static EntityIndex getIndex(EntityID);
static EntityVersion getVersion(EntityID);
static struct entity* getEntityStruct(struct scene*, EntityID);
static ComponentID getComponentID(struct scene* scene, ComponentKey globalKey);

struct scene* Scene_Create(void(registerComponents)(struct scene*), void (*update)(struct scene*), void (*render)(struct scene*), void (*destructor)(struct scene*))
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

    if (registerComponents) {
        registerComponents(retval);
    }

    return retval;
}

void Scene_Destroy(struct scene* scene)
{
    if (scene == NULL) {
        printf("WARNING: scene was NULL!"); // Not necessarily an error, but unlikely to happen during normal operation
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
            struct entity* entt = (struct entity*)Arraylist_Get(scene->entities, i);
        }
        struct entity* entt = (struct entity*)Arraylist_Get(scene->entities, getIndex(id));
        PANIC("Entity %d does not have component %d, mask is %d. Scene: %p", id >> 16, componentID, entt->mask, scene); // Sometimes entt is an invalid address and throws access violation, from BuyX(), from AIInfantryBuild(). Also from ProduceResources()
    } else if (getVersion(((struct entity*)Arraylist_Get(scene->entities, getIndex(id)))->id) != getVersion(id)) {
        PANIC("Outdated EntityID. Version is %d, given version was %d.\n\nThis means the EntityID that was provided to this function has been purged and no longer exists in the scene (or the given EntityID was garbled).\n\nWhile the index might still be the same, the version is incremented to avoid issues like this.\n\nCheck whether or not the place you got the EntityID from was valid. Chances are, it's stale data.", getVersion(((struct entity*)Arraylist_Get(scene->entities, getIndex(id)))->id), getVersion(id));
    } else {
        return Arraylist_Get(scene->components[componentID], getIndex(id));
    }
}

EntityID Scene_NewEntity(struct scene* scene)
{
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    } else if (scene->entities->size > MAX_ENTITIES) {
        PANIC("Entity overflow");
    }

    if (scene->freeIndices->size != 0) {
        EntityIndex index = *(EntityIndex*)Arraylist_Pop(scene->freeIndices);
        struct entity* entity = (struct entity*)Arraylist_Get(scene->entities, index);
        entity->id = ((EntityID)index << 16) | getVersion(entity->id) + 1;
        entity->mask = 0;
        scene->numEntities++;
        return entity->id;
    } else {
        EntityIndex index = (EntityIndex)(scene->entities->size);
        struct entity newEntity = { ((EntityID)index << 16 | 1), 0 };
        Arraylist_Add(&scene->entities, &newEntity);
        scene->numEntities++;
        for (int i = 0; i < scene->numComponents; i++) {
            Arraylist_AssertSize(&scene->components[i], index + 1);
        }
        return newEntity.id;
    }
}

void Scene_Assign(struct scene* scene, EntityID id, ComponentKey globalKey, void* componentData)
{
    ComponentID componentID = getComponentID(scene, globalKey);
    EntityIndex index = getIndex(id);
    EntityVersion version = getVersion(id);
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
    } else if (getVersion(((struct entity*)Arraylist_Get(scene->entities, index))->id) != version) {
        PANIC("Outdated EntityID. Version is %d, given version was %d.\n\nThis means the EntityID that was provided to this function has been purged and no longer exists in the scene (or the given EntityID was garbled).\n\nWhile the index might still be the same, the version is incremented to avoid issues like this.\n\nCheck whether or not the place you got the EntityID from was valid. Chances are, it's stale data.", getVersion(((struct entity*)Arraylist_Get(scene->entities, getIndex(id)))->id), getVersion(id));
    } else {
        getEntityStruct(scene, id)->mask |= ((ComponentMask)1 << componentID);
        Arraylist_Put(scene->components[componentID], getIndex(id), componentData);
    }
}

void Scene_Unassign(struct scene* scene, EntityID id, ComponentKey globalKey)
{
    ComponentID componentID = getComponentID(scene, globalKey);
    EntityIndex index = getIndex(id);
    EntityVersion version = getVersion(id);
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    } else if (getIndex(id) == INVALID_ENTITY_INDEX || getIndex(id) > MAX_ENTITIES) {
        PANIC("Invalid entity index");
    } else if (componentID >= MAX_COMPONENTS || componentID < 0) {
        PANIC("Invalid entity index");
    } else if (scene->components[componentID] == NULL) {
        PANIC("Component not registered yet");
    } else if (getVersion(((struct entity*)Arraylist_Get(scene->entities, index))->id) != version) {
        PANIC("Outdated EntityID. Version is %d, given version was %d.\n\nThis means the EntityID that was provided to this function has been purged and no longer exists in the scene (or the given EntityID was garbled).\n\nWhile the index might still be the same, the version is incremented to avoid issues like this.\n\nCheck whether or not the place you got the EntityID from was valid. Chances are, it's stale data.", getVersion(((struct entity*)Arraylist_Get(scene->entities, getIndex(id)))->id), getVersion(id));
    } else {
        getEntityStruct(scene, id)->mask &= ~((ComponentMask)1 << componentID);
    }
}

void Scene_MarkPurged(struct scene* scene, EntityID id)
{
    EntityIndex index = getIndex(id);
    EntityVersion version = getVersion(id);
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    } else if (getVersion(((struct entity*)Arraylist_Get(scene->entities, index))->id) != version) {
        PANIC("Outdated EntityID. Version is %d, given version was %d.\n\nThis means the EntityID that was provided to this function has been purged and no longer exists in the scene (or the given EntityID was garbled).\n\nWhile the index might still be the same, the version is incremented to avoid issues like this.\n\nCheck whether or not the place you got the EntityID from was valid. Chances are, it's stale data.", getVersion(((struct entity*)Arraylist_Get(scene->entities, getIndex(id)))->id), getVersion(id));
    }
    Arraylist_Add(&scene->purgedEntities, &index);
}

void Scene_Purge(struct scene* scene)
{
    while (scene->purgedEntities->size > 0) {
        EntityIndex index = *(EntityIndex*)Arraylist_Pop(scene->purgedEntities);
        struct entity* purgedEntity = (struct entity*)Arraylist_Get(scene->entities, index);
        purgedEntity->id = (INVALID_ENTITY_INDEX << 16) | getVersion(purgedEntity->id);
        purgedEntity->mask = 0;
        scene->numEntities--;
        // must gaurd against duplicates, otherwise allocated entity would be marked "free"
        if (!Arraylist_Contains(scene->freeIndices, &index)) {
            Arraylist_Add(&scene->freeIndices, &index);
        }
    }
}

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

bool Scene_EntityHasComponentMask(struct scene* scene, const ComponentMask mask, EntityID id)
{
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    }
    EntityIndex index = getIndex(id);
    EntityVersion version = getVersion(id);
    if (index > scene->entities->size) {
        PANIC("Malformed EntityID (i: %d | v: %d)", getIndex(id), getVersion(id));
    } else if (getVersion(((struct entity*)Arraylist_Get(scene->entities, index))->id) != version) {
        PANIC("Outdated EntityID. Version is %d, given version was %d.\n\nThis means the EntityID that was provided to this function has been purged and no longer exists in the scene (or the given EntityID was garbled).\n\nWhile the index might still be the same, the version is incremented to avoid issues like this.\n\nCheck whether or not the place you got the EntityID from was valid. Chances are, it's stale data. ticks: %d", getVersion(((struct entity*)Arraylist_Get(scene->entities, getIndex(id)))->id), getVersion(id), Apricot_Ticks);
    }
    struct entity* entt = (struct entity*)Arraylist_Get(scene->entities, index);
    return (entt->mask & mask) == mask;
}

bool Scene_EntityHasAnyComponents(struct scene* scene, const ComponentMask mask, EntityID id)
{
    EntityIndex index = getIndex(id);
    EntityVersion version = getVersion(id);
    if (!scene->valid) {
        PANIC("Scene is invalid, possibly recently destroyed.");
    } else if (getVersion(((struct entity*)Arraylist_Get(scene->entities, index))->id) != version) {
        PANIC("Outdated EntityID. Version is %d, given version was %d.\n\nThis means the EntityID that was provided to this function has been purged and no longer exists in the scene (or the given EntityID was garbled).\n\nWhile the index might still be the same, the version is incremented to avoid issues like this.\n\nCheck whether or not the place you got the EntityID from was valid. Chances are, it's stale data.", getVersion(((struct entity*)Arraylist_Get(scene->entities, getIndex(id)))->id), getVersion(id));
    }
    struct entity* entt = (struct entity*)Arraylist_Get(scene->entities, index);
    return (entt->mask & mask) != 0;
}

EntityID Scene_Begin(struct scene* scene, const ComponentMask mask)
{
    return Scene_Next(scene, -1, mask);
}

bool Scene_End(struct scene* scene, EntityID id)
{
    return getIndex(id) < scene->entities->size;
}

EntityID Scene_Next(struct scene* scene, EntityID prev, const ComponentMask mask)
{
    EntityIndex index;
    int i = 0;
    for (index = getIndex(prev) + 1; index < scene->entities->size; index++) {
        struct entity* entt = (struct entity*)Arraylist_Get(scene->entities, index);
        if (getIndex(entt->id) != INVALID_ENTITY_INDEX && (entt->mask & mask) == mask) {
            return entt->id;
        }
    }
    return (EntityID)(scene->entities->size) << 16;
}

/**
 * @brief Returns the index portion of an EntityID
 * @param id The id to get the index from
 * @return The index of the given EntityID
*/
EntityIndex getIndex(EntityID id)
{
    return (id >> 16) & 0xFFFF;
}

/**
 * @brief Returns the version portion of an EntityID
 * @param id The id to get the version from
 * @return The version of a given EntityID
*/
EntityVersion getVersion(EntityID id)
{
    return id & 0xFFFF;
}

/**
 * @brief Returns the entity structure in a scene's arraylist of entities for a given EntityID
 * @param scene	The relevant scene
 * @param id	The id retrieve from
*/
struct entity* getEntityStruct(struct scene* scene, EntityID id)
{
    return (struct entity*)Arraylist_Get(scene->entities, getIndex(id));
}

/**
 * @brief Maps a ComponentKey to its corresponding ComponentID in a scene
 * @param scene The relevant scene
 * @param globalKey The component key
 * @return The corresponding component id
*/
static ComponentID getComponentID(struct scene* scene, ComponentKey globalKey)
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
    return INVALID_COMPONENT_ID;
}