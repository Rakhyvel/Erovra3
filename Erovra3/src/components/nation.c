#pragma once
#include "components.h"
#include "nation.h"

EntityID Nation_Create(struct scene* scene, SDL_Color color)
{
    EntityID nationID = Scene_NewEntity(scene);
    Nation nation = {
        color
    };
    Scene_Assign(scene, nationID, NATION_COMPONENT_ID, &nation);
    return nationID;
}