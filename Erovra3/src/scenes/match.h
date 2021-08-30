#pragma once
#include "../engine/scene.h"
#include "../terrain.h"

extern Terrain* terrain;

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

void Match_Render(Scene* scene);
Scene* Match_Init(float* map, int mapSize, bool AIControlled);