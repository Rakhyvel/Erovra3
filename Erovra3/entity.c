#pragma once
#include <stdio.h>
#include <string.h>
#include "entity.h"
#include "debug.h"

static int getIndex(EntityID id);
static int getVersion(EntityID id);
static struct entity* getEntityStruct(EntityID id);

void ECS_Init()
{
	entities = arraylist_create(sizeof(struct entity));
}


void ECS_RegisterComponent(int componentID, size_t componentSize) 
{
	if (components[componentID] != NULL)
	{
		PANIC("Component already registered");
	}
	else
	{
		components[componentID] = arraylist_create(componentSize);
	}
}

void* ECS_GetComponent(EntityID id, int componentID)
{
	return arraylist_get(components[componentID], getIndex(id));
}

EntityID ECS_NewID()
{
	EntityID id = ((unsigned long long)(entities->size) << 32) | 0;
	struct entity temp = {
		id,
		0
	};
	arraylist_add(entities, &temp);
	return id;
}

void ECS_Assign(EntityID id, int componentID, void* data) {
	getEntityStruct(id)->mask |= (1 << componentID);
	arraylist_put(components[componentID], getIndex(id), data);
}

// returns the index portion of an id
int getIndex(EntityID id)
{
	return (id >> 32);
}

// returns the version portion of an id
int getVersion(EntityID id)
{
	return id & 0xFFFFFFFF;
}

struct entity* getEntityStruct(EntityID id)
{
	return ((struct entity*)(entities->data + getIndex(id)));
}
