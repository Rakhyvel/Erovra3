#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

EntityID Nation_Create(struct scene* scene, SDL_Color color, ComponentID homeNation, ComponentID enemyNation);