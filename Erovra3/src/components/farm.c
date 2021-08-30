#pragma once
#include "../components/components.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "./farm.h"

/*
	Creates a farm entity */
EntityID Farm_Create(struct scene* scene, Vector pos, EntityID nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID farmID = Scene_NewEntity(scene);
    // pos parameter will be slightly towards the home city, hopefully
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false
    };
    Scene_Assign(scene, farmID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        FARM_TEXTURE_ID,
        BUILDING_OUTLINE_TEXTURE_ID,
        INVALID_TEXTURE_ID,
        RenderPriorirty_BUILDING_LAYER,
        false,
        false,
        nation,
        32,
        32,
        36,
        36
    };
    Scene_Assign(scene, farmID, BUILDING_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, farmID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, farmID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_FARM,
        1
    };
    Scene_Assign(scene, farmID, UNIT_COMPONENT_ID, &type);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, farmID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        BUILDING_FOCUSED_GUI
    };
    Scene_Assign(scene, farmID, FOCUSABLE_COMPONENT_ID, &focusable);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, farmID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, farmID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, farmID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, farmID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return farmID;
}
