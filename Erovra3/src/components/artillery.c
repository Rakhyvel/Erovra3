#pragma once
#include "../scenes/match.h"
#include "../textures.h"
#include "components.h"
#include "shell.h"

/*
	Creates an artillery entity */
EntityID Artillery_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID artilleryID = Scene_NewEntity(scene);

    Nation* nationStruct = (Unit*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

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
        Scene_CreateMask(3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, artilleryID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_ARTILLERY,
        1.0f
    };
    Scene_Assign(scene, artilleryID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        0.8f,
        100.0f,
        Scene_CreateMask(2, LAND_UNIT_FLAG_COMPONENT_ID, nationStruct->enemyNationFlag),
        120,
        &Shell_Create,
		true
    };
    Scene_Assign(scene, artilleryID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, artilleryID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, artilleryID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, artilleryID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, artilleryID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, artilleryID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, artilleryID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, artilleryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, artilleryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return artilleryID;
}
