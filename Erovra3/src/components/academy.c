#pragma once
#include "../components/components.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "./academy.h"

/*
	Creates a academy entity */
EntityID Academy_Create(struct scene* scene, Vector pos, EntityID nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID academyID = Scene_NewEntity(scene);
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false
    };
    Scene_Assign(scene, academyID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        ACADEMY_TEXTURE_ID,
        BUILDING_OUTLINE_TEXTURE_ID,
        ACADEMY_SHADOW_TEXTURE_ID,
		RenderPriorirty_BUILDING_LAYER,
        false,
        false,
        nation,
        32,
        32,
        36,
        36
    };
    Scene_Assign(scene, academyID, BUILDING_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, academyID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, academyID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_ACADEMY,
        1
    };
    Scene_Assign(scene, academyID, UNIT_COMPONENT_ID, &type);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, academyID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        ACADEMY_READY_FOCUSED_GUI
    };
    Scene_Assign(scene, academyID, FOCUSABLE_COMPONENT_ID, &focusable);

    Producer producer = {
        -1,
        INVALID_ENTITY_INDEX,
        false,
        ACADEMY_READY_FOCUSED_GUI,
        ACADEMY_BUSY_FOCUSED_GUI
    };
    Scene_Assign(scene, academyID, PRODUCER_COMPONENT_ID, &producer);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, academyID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, academyID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, academyID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, academyID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return academyID;
}
