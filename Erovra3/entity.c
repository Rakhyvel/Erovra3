/*	entity.c

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
#include <string.h>
#include "entity.h"
#include "debug.h"

static EntityIndex getIndex(EntityID);
static EntityVersion getVersion(EntityID);
static struct entity* getEntityStruct(EntityID);

/*
	Initializes the memory pools for a scene */
void ECS_Init()
{
	entities = arraylist_create(sizeof(struct entity));
	purgedEntities = arraylist_create(sizeof(EntityIndex));
	freeIndices = arraylist_create(sizeof(EntityIndex));
	numEntities = 0;
}

/*
	Assigns a size to a component, allocates memory pool for component data.
	
	Cannot register on same componentID more than once. */
void ECS_RegisterComponent(ComponentID componentID, size_t componentSize) 
{
	if (components[componentID] != NULL)
	{
		PANIC("Component already registered");
	}
	else if (numEntities > 0)
	{
		PANIC("Cannot create component if entities exist in scene %d", numEntities);
	}
	else
	{
		components[componentID] = arraylist_create(componentSize);
	}
}

/*
	Returns a pointer to the component data for a given entity and a given component.
	
	EntityID must be valid, componentID must be valid, and entity must have component  */
void* ECS_GetComponent(EntityID id, ComponentID componentID)
{
	if (getIndex(id) == INVALID_ENTITY_INDEX)
	{
		PANIC("Invalid entity index");
	}
	else if (!(ARRAYLIST_GET(entities, getIndex(id), struct entity)->mask & (ComponentMask)((ComponentMask)1 << componentID)))
	{
		PANIC("Entity does not have component %d", componentID);
	}
	else if (componentID >= MAX_COMPS || componentID < 0)
	{
		PANIC("Invalid entity index");
	}
	else if (components[componentID] == NULL) 
	{
		PANIC("Component id not registered yet");
	}
	else
	{
		return arraylist_get(components[componentID], getIndex(id));
	}
}

/*
	Allocates and returns a new EntityID. Will either reallocate a freed 
	reference, or create a new one.
	
	Entities are gauranteed to have a unique index-version number pairing
	than all other entities created before.
	
	Entities are gauranteed to have a clear component mask. */
EntityID ECS_NewID()
{
	numEntities++;
	if (freeIndices->size != 0)
	{
		EntityIndex index = ARRAYLIST_POP_DEREF(freeIndices, EntityIndex);
		struct entity* entity = ARRAYLIST_GET(entities, index, struct entity);
		entity->id = ((EntityID)index << 32) | getVersion(entity->id);
		return entity->id;
	}
	else
	{
		EntityIndex index = (EntityIndex)(entities->size);
		struct entity newEntity = { index, 0 };
		arraylist_add(entities, &newEntity);
		return index;
	}
}

/*
	Assigns a component to an entity. Double assignment is ok.

	If given componentData argument is NULL, only the mask will be changed.
	ComponentID does not need to be registered. Component can then act as a flag.
	
	If given componentData argument is not NULL, component must be registered.
	Data will then be copied from dereference of pointer to component data
	memory pool. */
void ECS_Assign(EntityID id, ComponentID componentID, void* componentData)
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
		getEntityStruct(id)->mask |= ((ComponentMask)1 << componentID);
	}
	else if (components[componentID] == NULL)
	{
		PANIC("Component id not registered yet");
	}
	else
	{
		getEntityStruct(id)->mask |= ((ComponentMask)1 << componentID);
		arraylist_put(components[componentID], getIndex(id), componentData);
	}
}

/*
	Marks an entity as purged, and ready for deallocation when the purge 
	function is called */
void ECS_MarkPurged(EntityID id)
{
	arraylist_add(purgedEntities, &id);
}

/*
	Frees all purged entities.
	
	Marks index as invalid, increments version, clears mask, and finally frees
	the index in the entity thing */
void ECS_Purge() 
{
	while (purgedEntities->size != 0)
	{
		EntityID id = ARRAYLIST_POP_DEREF(purgedEntities, EntityID);
		EntityIndex index = getIndex(id);
		EntityVersion version = getVersion(id);
		struct entity* purgedEntity = ARRAYLIST_GET(entities, index, struct entity);
		purgedEntity->id = (INVALID_ENTITY_INDEX << 32) | ++version;
		purgedEntity->mask = 0;
		arraylist_add(freeIndices, &index);
		numEntities--;
	}
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
struct entity* getEntityStruct(EntityID id)
{
	return ((struct entity*)(entities->data + getIndex(id)));
}
