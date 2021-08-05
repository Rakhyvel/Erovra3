#pragma once
#include "../scenes/match.h"
#include "../textures.h"
#include "./shell.h"
#include "bullet.h"
#include "components.h"

/*
	Creates an entity with battleship components */
EntityID Battleship_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID battleshipID = Scene_NewEntity(scene);
    Nation* nationStruct = (Unit*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

    Motion motion = {
        pos,
        0.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.1f,
        false
    };
    Scene_Assign(scene, battleshipID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, battleshipID, TARGET_COMPONENT_ID, &target);

    SimpleRenderable render = {
        BATTLESHIP_TEXTURE_ID,
        SHIP_OUTLINE_TEXTURE_ID,
        SHIP_SHADOW_TEXTURE_ID,
        false,
        false,
        nation,
        16,
        46,
        16,
        46
    };
    Scene_Assign(scene, battleshipID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
        Scene_CreateMask(3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, battleshipID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_BATTLESHIP,
        2.0f
    };
    Scene_Assign(scene, battleshipID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        4.0f,
        250.0f,
        Scene_CreateMask(2, SHIP_FLAG_COMPONENT_ID, nationStruct->enemyNationFlag),
        120,
        &Shell_Create,
		false
    };
    Scene_Assign(scene, battleshipID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, battleshipID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, battleshipID, HOVERABLE_COMPONENT_ID, &hoverable);

    Scene_Assign(scene, battleshipID, SHIP_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, battleshipID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, battleshipID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, battleshipID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return battleshipID;
}
