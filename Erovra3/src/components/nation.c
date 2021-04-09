#pragma once
#include "components.h"
#include "nation.h"

/*
	Takes in a scene and color, registers a nation entity, assigns components 
	relevant to a nation. Return nation's EntityID */
EntityID Nation_Create(struct scene* scene, SDL_Color color)
{
    EntityID nationID = Scene_NewEntity(scene);
    Nation nation = {
        color
    };
    Scene_Assign(scene, nationID, NATION_COMPONENT_ID, &nation);
    return nationID;
}