#pragma once
#include "../engine/scene.h"
#include "../terrain.h"
#include "../util/sortedlist.h"

enum RenderPriority {
    RenderPriorirty_BUILDING_LAYER,
    RenderPriorirty_SURFACE_LAYER,
	RenderPriority_HIGH_SURFACE_LAYER,
    RenderPriorirty_AIR_LAYER,
    RenderPriorirty_PARTICLE_LAYER,
};

extern Terrain* terrain;

extern SortedList renderList;

EntityID ENGINEER_FOCUSED_GUI;

// Default containers
EntityID BUILDING_FOCUSED_GUI;
EntityID UNIT_FOCUSED_GUI;

EntityID FACTORY_READY_FOCUSED_GUI;
EntityID FACTORY_BUSY_FOCUSED_GUI;

EntityID PORT_READY_FOCUSED_GUI;
EntityID PORT_BUSY_FOCUSED_GUI;

EntityID ACADEMY_READY_FOCUSED_GUI;
EntityID ACADEMY_BUSY_FOCUSED_GUI;

Scene* Match_Init();