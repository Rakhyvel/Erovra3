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
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "scene.h"
#include "../util/debug.h"

static EntityIndex getIndex(EntityID);
static EntityVersion getVersion(EntityID);
static struct entity* getEntityStruct(struct scene*, EntityID);

/*
	Initializes the memory pools for a scene */
struct scene* Scene_Create(void (initComponents)(struct Scene*))
{
	struct scene* retval = calloc(1, sizeof(struct scene));
	if (!retval)
	{
		PANIC("Memory error");
	}

	retval->entities = Arraylist_Create(10, sizeof(struct entity));
	retval->purgedEntities = Arraylist_Create(10, sizeof(EntityIndex));
	retval->freeIndices = Arraylist_Create(10, sizeof(EntityIndex));

	initComponents(retval);

	return retval;
}

/*
	Assigns a size to a component, allocates memory pool for component data.
	
	Cannot register on same componentID more than once. */
const ComponentID Scene_RegisterComponent(struct scene* scene, size_t componentSize)
{
	const ComponentID componentID = scene->numComponents + 1;
	if (scene->numComponents > MAX_COMPONENTS)
	{
		PANIC("Component overflow");
	}
	else if (scene->numEntities > 0)
	{
		PANIC("Cannot create component if entities exist in scene %d", scene->numEntities);
	}
	else
	{
		scene->components[componentID] = Arraylist_Create(MAX_ENTITIES, componentSize);
		scene->numComponents++;
	}
	return componentID;
}

/*
	Returns a pointer to the component data for a given entity and a given component.
	
	EntityID must be valid, componentID must be valid, and entity must have component  */
void* Scene_GetComponent(struct scene* scene, EntityID id, ComponentID componentID)
{
	if (getIndex(id) == INVALID_ENTITY_INDEX)
	{
		PANIC("Invalid entity index");
	}
	else if (componentID >= MAX_COMPONENTS || componentID < 0)
	{
		PANIC("Invalid entity index");
	}
	else if (scene->components[componentID] == NULL)
	{
		PANIC("Component id not registered yet");
	}
	else
	{
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
	scene->numEntities++;
	if (scene->numEntities > MAX_ENTITIES)
	{
		fprintf(stderr, "Entity overflow");
		exit(1);
	}

	if (scene->freeIndices->size != 0)
	{
		EntityIndex index = ARRAYLIST_POP_DEREF(scene->freeIndices, EntityIndex);
		struct entity* entity = ARRAYLIST_GET(scene->entities, index, struct entity);
		entity->id = ((EntityID)index << sizeof(EntityVersion)) | getVersion(entity->id);
		return entity->id;
	}
	else
	{
		EntityIndex index = (EntityIndex)(scene->entities->size);
		struct entity newEntity = { ((EntityID)index << 16), 0 };
		Arraylist_Add(scene->entities, &newEntity);
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
void Scene_Assign(struct scene* scene, EntityID id, ComponentID componentID, void* componentData)
{
	if (getIndex(id) == INVALID_ENTITY_INDEX || getIndex(id) > MAX_ENTITIES) 
	{
		PANIC("Invalid entity index");
	}
	else if (componentID >= MAX_COMPONENTS || componentID < 0)
	{
		PANIC("Invalid entity index");
	}
	else if (componentData == NULL) 
	{
		getEntityStruct(scene, id)->mask |= ((ComponentMask)1 << componentID);
	}
	else if (scene->components[componentID] == NULL)
	{
		PANIC("Component id not registered yet");
	}
	else
	{
		getEntityStruct(scene, id)->mask |= ((ComponentMask)1 << componentID);
		Arraylist_Put(scene->components[componentID], getIndex(id), componentData);
	}
}

/*
	Marks an entity as purged, and ready for deallocation when the purge 
	function is called */
void Scene_MarkPurged(struct scene* scene, EntityID id)
{
	Arraylist_Add(scene->purgedEntities, &id);
}

/*
	Frees all purged entities.
	
	Marks index as invalid, increments version, clears mask, and finally frees
	the index in the entity thing */
void Scene_Purge(struct scene* scene)
{
	while (scene->purgedEntities->size != 0)
	{
		EntityID id = ARRAYLIST_POP_DEREF(scene->purgedEntities, EntityID);
		EntityIndex index = getIndex(id);
		EntityVersion version = getVersion(id);
		struct entity* purgedEntity = ARRAYLIST_GET(scene->entities, index, struct entity);
		purgedEntity->id = (INVALID_ENTITY_INDEX << 16) | ++version;
		purgedEntity->mask = 0;
		Arraylist_Add(scene->freeIndices, &index);
		scene->numEntities--;
	}
}

/*
	Creates a component bit mask based on given a list of component ids */
ComponentMask Scene_CreateMask(int number, ComponentID components, ...)
{
	ComponentID component = components;
	ComponentMask retval = 0;
	va_list args;
	va_start(args, components);

	for (int i = 0; i < number; i++)
	{
		retval |= ((ComponentMask)1 << (ComponentMask)component);
		component = va_arg(args, ComponentID);
	}

	va_end(args);

	return retval;
}

/*
	Finds and returns first index of a mask matching an entity */
EntityID Scene_Begin(struct scene* scene, ComponentMask mask)
{
	return Scene_Next(scene, -1, mask);
}

/*
	Checks to see if the entity id is the last entity */
bool Scene_End(struct scene* scene, EntityID id)
{
	return getIndex(id) != scene->numEntities;
}

/*
	Returns the next entity id given a previous entity id, and a component mask */
EntityID Scene_Next(struct scene* scene, EntityID prev, ComponentMask mask)
{
	EntityIndex index;
	for (index = getIndex(prev) + 1; index < scene->entities->size; index++)
	{
		struct entity* entt = ARRAYLIST_GET(scene->entities, index, struct entity);
		if (entt->id != INVALID_ENTITY_INDEX &&
			(entt->mask & mask))
		{
			return entt->id;
		}
	}
	return (EntityID)(scene->entities->size) << 16;
}

// returns the index portion of an id
EntityIndex getIndex(EntityID id)
{
	return (id >> 16);
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