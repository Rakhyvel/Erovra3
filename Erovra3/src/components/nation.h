#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"
#include "../engine/goap.h"

EntityID Nation_Create(struct scene* scene, Goap* goap, SDL_Color color, int mapSize, ComponentKey homeNation, ComponentKey enemyNation, ComponentKey controlFlag);
void Nation_SetCapital(struct scene* scene, EntityID nationID, EntityID capital);