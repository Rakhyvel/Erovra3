#pragma once
#include "../textures.h"
#include "coin.h"
#include "components.h"

EntityID Ore_Create(struct scene* scene, Vector pos, EntityID nationID)
{
    EntityID oreID = Scene_NewEntity(scene);

    Nation* nation = (Nation*)Scene_GetComponent(scene, nationID, NATION_COMPONENT_ID);
    Motion* capitalMotion = (Motion*)Scene_GetComponent(scene, nation->capital, MOTION_COMPONENT_ID);
    Vector vel = Vector_Sub(capitalMotion->pos, pos);
    vel = Vector_Normalize(vel);
    vel = Vector_Scalar(vel, 6);
    float angle = Vector_Angle(vel);
    Motion motion = {
        pos,
        0.5f,
        vel,
        0,
        0.2f,
        true
    };
    Scene_Assign(scene, oreID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        ORE_TEXTURE_ID,
        INVALID_TEXTURE_ID,
        INVALID_TEXTURE_ID,
        false,
        nationID,
        20,
        20,
        0,
        0
    };
    Scene_Assign(scene, oreID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Scene_Assign(scene, oreID, ORE_COMPONENT_ID, NULL);
    Scene_Assign(scene, oreID, GET_COMPONENT_FIELD(scene, nationID, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return oreID;
}