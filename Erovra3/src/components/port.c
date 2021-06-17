#pragma once
#include "../components/components.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "./factory.h"

/*
	Creates a port entity */
EntityID Port_Create(struct scene* scene, Vector pos, EntityID nation, EntityID homeCity)
{
    EntityID portID = Scene_NewEntity(scene);
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false
    };
    Scene_Assign(scene, portID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        (struct vector) { 0.0f, 0.0f },
        pos,
    };
    Scene_Assign(scene, portID, TARGET_COMPONENT_ID, &target);

    SimpleRenderable render = {
        PORT_TEXTURE_ID,
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
    Scene_Assign(scene, portID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
    };
    Scene_Assign(scene, portID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_FACTORY,
        1
    };
    Scene_Assign(scene, portID, UNIT_COMPONENT_ID, &type);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, portID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        PORT_READY_FOCUSED_GUI
    };
    Scene_Assign(scene, portID, FOCUSABLE_COMPONENT_ID, &focusable);

    Producer producer = {
        -1,
        INVALID_ENTITY_INDEX,
        false,
        homeCity,
        PORT_READY_FOCUSED_GUI,
        PORT_BUSY_FOCUSED_GUI
    };
    Scene_Assign(scene, portID, PRODUCER_COMPONENT_ID, &producer);

    Scene_Assign(scene, portID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, portID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, portID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, portID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return portID;
}