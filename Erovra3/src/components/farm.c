#pragma once
#include "../components/components.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "./airfield.h"

/*
	Creates a airfield entity */
EntityID Airfield_Create(struct scene* scene, Vector pos, EntityID nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID airfieldID = Scene_NewEntity(scene);
    // pos parameter will be slightly towards the home city, hopefully
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false
    };
    Scene_Assign(scene, airfieldID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        AIRFIELD_TEXTURE_ID,
        BUILDING_OUTLINE_TEXTURE_ID,
        INVALID_TEXTURE_ID,
        false,
        false,
        nation,
        32,
        32,
        36,
        36
    };
    Scene_Assign(scene, airfieldID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
        Scene_CreateMask(3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, airfieldID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_AIRFIELD,
        1
    };
    Scene_Assign(scene, airfieldID, UNIT_COMPONENT_ID, &type);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, airfieldID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        FACTORY_READY_FOCUSED_GUI
    };
    Scene_Assign(scene, airfieldID, FOCUSABLE_COMPONENT_ID, &focusable);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, airfieldID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, airfieldID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, airfieldID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, airfieldID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return airfieldID;
}