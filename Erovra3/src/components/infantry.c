#pragma once
#include "../textures.h"
#include "../scenes/match.h"
#include "components.h"

/*
	Takes in a scene and some information relevant to infantry entities, registers
	a new entity, assigns components to that infantry entity. Returns the EntityID
	of the infantry */
EntityID Infantry_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID infantryID = Scene_NewEntity(scene);
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        1.2f,
        false
    };
    Scene_Assign(scene, infantryID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, infantryID, TARGET_COMPONENT_ID, &target);

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
    Scene_Assign(scene, infantryID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
    };
    Scene_Assign(scene, infantryID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
		UnitType_INFANTRY,
        1,
        0.5f
    };
    Scene_Assign(scene, infantryID, UNIT_COMPONENT_ID, &type);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, infantryID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, infantryID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        INFANTRY_FOCUSED_GUI
    };
    Scene_Assign(scene, infantryID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, infantryID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, infantryID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, infantryID, BULLET_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, infantryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    return infantryID;
}
