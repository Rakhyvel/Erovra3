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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "scene.h"
#include "debug.h"

static EntityIndex getIndex(EntityID);
static EntityVersion getVersion(EntityID);
static struct entity* getEntityStruct(struct scene*, EntityID);

/*
	Initializes the memory pools for a scene */
struct scene* Scene_Create()
{
	struct scene* retval = calloc(1, sizeof(struct scene));
	if (!retval)
	{
		PANIC("Memory error");
	}

	retval->entities = arraylist_create(sizeof(struct entity));
	retval->purgedEntities = arraylist_create(sizeof(EntityIndex));
	retval->freeIndices = arraylist_create(sizeof(EntityIndex));

	return retval;
}

/*
	Assigns a size to a component, allocates memory pool for component data.
	
	Cannot register on same componentID more than once. */
void Scene_RegisterComponent(struct scene* scene, ComponentID componentID, size_t componentSize)
{
	if (scene->components[componentID] != NULL)
	{
		PANIC("Component %d already registered", componentID);
	}
	else if (scene->numEntities > 0)
	{
		PANIC("Cannot create component if entities exist in scene %d", scene->numEntities);
	}
	else
	{
		scene->components[componentID] = arraylist_create(componentSize);
	}
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
	else if (componentID >= MAX_COMPS || componentID < 0)
	{
		PANIC("Invalid entity index");
	}
	else if (scene->components[componentID] == NULL)
	{
		PANIC("Component id not registered yet");
	}
	else
	{
		return arraylist_get(scene->components[componentID], getIndex(id));
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
	if (scene->freeIndices->size != 0)
	{
		EntityIndex index = ARRAYLIST_POP_DEREF(scene->freeIndices, EntityIndex);
		struct entity* entity = ARRAYLIST_GET(scene->entities, index, struct entity);
		entity->id = ((EntityID)index << 32) | getVersion(entity->id);
		return entity->id;
	}
	else
	{
		EntityIndex index = (EntityIndex)(scene->entities->size);
		struct entity newEntity = { ((EntityID)index << 32), 0 };
		arraylist_add(scene->entities, &newEntity);
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
	if (getIndex(id) == INVALID_ENTITY_INDEX) 
	{
		PANIC("Invalid entity index");
	}
	else if (componentID >= MAX_COMPS || componentID < 0)
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
		if (scene->components[componentID]->size < getIndex(id))
		{
			void* res = realloc(scene->components[componentID], getIndex(id)+1);
			if (!res)
			{
				free(scene->components[componentID]);
				PANIC("Memory error");
			}
			else
			{
				scene->components[componentID] = res;
			}
		}
		arraylist_put(scene->components[componentID], getIndex(id), componentData);
	}
}

/*
	Marks an entity as purged, and ready for deallocation when the purge 
	function is called */
void Scene_MarkPurged(struct scene* scene, EntityID id)
{
	arraylist_add(scene->purgedEntities, &id);
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
		purgedEntity->id = (INVALID_ENTITY_INDEX << 32) | ++version;
		purgedEntity->mask = 0;
		arraylist_add(scene->freeIndices, &index);
		scene->numEntities--;
	}
}

ComponentMask Scene_CreateMask(int number, ComponentID components, ...)
{
	ComponentID component = components;
	ComponentMask retval = 0;
	va_list args;
	va_start(args, components);

	for (int i = 0; i < number; i++)
	{
		printf("%d\n", component);
		retval |= ((ComponentMask)1 << (ComponentMask)component);
		component = va_arg(args, ComponentID);
	}

	va_end(args);

	return retval;
}

// returns the index portion of an id
EntityIndex getIndex(EntityID id)
{
	return (id >> 32);
}

// returns the version portion of an id
EntityVersion getVersion(EntityID id)
{
	return id & 0xFFFFFFFF;
}

// returns the entity structure for a given EntityID
struct entity* getEntityStruct(struct scene* scene, EntityID id)
{
	return ((struct entity*)(scene->entities->data + getIndex(id)));
}
