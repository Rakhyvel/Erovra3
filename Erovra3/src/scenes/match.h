#pragma once
#include "../engine/scene.h"
#include "../terrain.h"

extern Terrain* terrain;

EntityID INFANTRY_FOCUSED_GUI;
EntityID FACTORY_READY_FOCUSED_GUI;
EntityID FACTORY_BUSY_FOCUSED_GUI;

Scene* Match_Init();