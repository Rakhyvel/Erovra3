#pragma once
#include "./factory.h"
#include "../components/components.h"
#include "../scenes/match.h"
#include "../textures.h"

/*
	Creates a factory entity */
EntityID Factory_Create(struct scene* scene, Vector pos, EntityID nation, EntityID homeCity)
{
    EntityID factoryID = Scene_NewEntity(scene);
	// pos parameter will be slightly towards the home city, hopefully
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false
    };
    Scene_Assign(scene, factoryID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        FACTORY_TEXTURE_ID,
        BUILDING_OUTLINE_TEXTURE_ID,
        FACTORY_SHADOW_TEXTURE_ID,
        false,
        false,
        nation,
        32,
        32,
        36,
        36
    };
    Scene_Assign(scene, factoryID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
        Scene_CreateMask(3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, factoryID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_FACTORY,
        1
    };
    Scene_Assign(scene, factoryID, UNIT_COMPONENT_ID, &type);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, factoryID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        FACTORY_READY_FOCUSED_GUI
    };
    Scene_Assign(scene, factoryID, FOCUSABLE_COMPONENT_ID, &focusable);

    Producer producer = {
        -1,
        INVALID_ENTITY_INDEX,
        false,
        homeCity,
        FACTORY_READY_FOCUSED_GUI,
        FACTORY_BUSY_FOCUSED_GUI
    };
    Scene_Assign(scene, factoryID, PRODUCER_COMPONENT_ID, &producer);

    Scene_Assign(scene, factoryID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, factoryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, factoryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return factoryID;
}
