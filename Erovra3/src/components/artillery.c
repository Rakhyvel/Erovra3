#pragma once
#include "../scenes/match.h"
#include "../textures.h"
#include "components.h"

EntityID Artillery_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID artilleryID = Scene_NewEntity(scene);
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.2f,
        false
    };
    Scene_Assign(scene, artilleryID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, artilleryID, TARGET_COMPONENT_ID, &target);

    SimpleRenderable render = {
        ARTILLERY_TEXTURE_ID,
        GROUND_OUTLINE_TEXTURE_ID,
        GROUND_SHADOW_TEXTURE_ID,
        false,
        nation,
        32,
        16,
        36,
        20
    };
    Scene_Assign(scene, artilleryID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
    };
    Scene_Assign(scene, artilleryID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_ARTILLERY,
        2,
        0.5f
    };
    Scene_Assign(scene, artilleryID, UNIT_COMPONENT_ID, &type);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, artilleryID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, artilleryID, HOVERABLE_COMPONENT_ID, &hoverable);

    Scene_Assign(scene, artilleryID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, artilleryID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, artilleryID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, artilleryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, artilleryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return artilleryID;
}
