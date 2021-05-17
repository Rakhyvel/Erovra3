#pragma once
#include "nation.h"
#include "components.h"

/*
	Takes in a scene and color, registers a nation entity, assigns components 
	relevant to a nation. Return nation's EntityID */
EntityID Nation_Create(struct scene* scene, SDL_Color color, ComponentID ownNation, ComponentID enemyNation)
{
    EntityID nationID = Scene_NewEntity(scene);
    Nation nation = {
        color,
        ownNation,
        enemyNation,
        45, // coins
        0, // ore
        1, // pop.
		5, // pop. cap
        5, // city cost
        10, // factory cost
        10, // mine cost
        15, // cavalry cost
        15 // artillery cost
    };
    Scene_Assign(scene, nationID, NATION_COMPONENT_ID, &nation);
    Scene_Assign(scene, nationID, ownNation, NULL);
    return nationID;
}