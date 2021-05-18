#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

EntityID Nation_Create(struct scene* scene, SDL_Color color, int mapSize, ComponentID homeNation, ComponentID enemyNation, ComponentID controlFlag);
void Nation_SetCapital(struct scene* scene, EntityID nationID, EntityID capital);