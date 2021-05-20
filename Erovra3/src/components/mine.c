#pragma once
#include "mine.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "components.h"

/*
	Creates a mine */
EntityID Mine_Create(struct scene* scene, Vector pos, EntityID nation)
{
    EntityID mineID = Scene_NewEntity(scene);
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false
    };
    Scene_Assign(scene, mineID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        (struct vector) { 0.0f, 0.0f },
        pos,
    };
    Scene_Assign(scene, mineID, TARGET_COMPONENT_ID, &target);

    SimpleRenderable render = {
        MINE_TEXTURE_ID,
        BUILDING_OUTLINE_TEXTURE_ID,
        BUILDING_SHADOW_TEXTURE_ID,
        false,
        false,
        nation,
        32,
        32,
        36,
        36
    };
    Scene_Assign(scene, mineID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
    };
    Scene_Assign(scene, mineID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_MINE,
        0.05
    };
    Scene_Assign(scene, mineID, UNIT_COMPONENT_ID, &type);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, mineID, HOVERABLE_COMPONENT_ID, &hoverable);

    Scene_Assign(scene, mineID, MINE_COMPONENT_ID, NULL);

    Scene_Assign(scene, mineID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, mineID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, mineID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, mineID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return mineID;
}