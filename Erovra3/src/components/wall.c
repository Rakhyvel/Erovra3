#pragma once
#include "../scenes/match.h"
#include "../textures.h"
#include "components.h"

/*
	Takes in a scene and some information relevant to wall entities, registers
	a new entity, assigns components to that wall entity. Returns the EntityID
	of the wall */
EntityID Wall_Create(Scene* scene, Vector pos, float angle, EntityID nation)
{
    EntityID wallID = Scene_NewEntity(scene);
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        angle,
        0.0f,
        false
    };
    Scene_Assign(scene, wallID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        WALL_TEXTURE_ID,
        INVALID_TEXTURE_ID,
        WALL_SHADOW_TEXTURE_ID,
		false,
        false,
        nation,
        64,
        64,
        36,
        36
    };
    Scene_Assign(scene, wallID, BUILDING_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, wallID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, wallID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_WALL,
        2.0f
    };
    Scene_Assign(scene, wallID, UNIT_COMPONENT_ID, &type);

    Scene_Assign(scene, wallID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, wallID, WALL_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, wallID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, wallID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return wallID;
}
