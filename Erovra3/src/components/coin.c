#pragma once
#include "coin.h"
#include "../textures.h"
#include "components.h"

EntityID Coin_Create(struct scene* scene, Vector pos, EntityID nation)
{
    EntityID coinID = Scene_NewEntity(scene);
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.2f,
        false
    };
    Scene_Assign(scene, coinID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        INFANTRY_TEXTURE_ID,
        GROUND_OUTLINE_TEXTURE_ID,
        GROUND_SHADOW_TEXTURE_ID,
        false,
        nation,
        32,
        16,
        36,
        20
    };
    Scene_Assign(scene, coinID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    return coinID;
}