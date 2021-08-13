#pragma once
#include "../engine/scene.h"
#include "../terrain.h"

extern Terrain* terrain;

EntityID ENGINEER_FOCUSED_GUI;

EntityID FACTORY_READY_FOCUSED_GUI;
EntityID FACTORY_BUSY_FOCUSED_GUI;

EntityID CITY_READY_FOCUSED_GUI;
EntityID CITY_BUSY_FOCUSED_GUI;

EntityID PORT_READY_FOCUSED_GUI;
EntityID PORT_BUSY_FOCUSED_GUI;

Scene* Match_Init();