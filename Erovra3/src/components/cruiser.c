#pragma once
#include "cruiser.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "./shell.h"
#include "bullet.h"
#include "components.h"

/*
	Creates an entity with cruiser components */
EntityID Cruiser_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID cruiserID = Scene_NewEntity(scene);
    Nation* nationStruct = (Unit*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

    Motion motion = {
        pos,
        0.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.2f, // speed
        false
    };
    Scene_Assign(scene, cruiserID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, cruiserID, TARGET_COMPONENT_ID, &target);

    SimpleRenderable render = {
        CRUISER_TEXTURE_ID,
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
    Scene_Assign(scene, cruiserID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
        Scene_CreateMask(2, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID)
    };
    Scene_Assign(scene, cruiserID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_CRUISER,
        1.0f
    };
    Scene_Assign(scene, cruiserID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        2.0f, // Attack
        250.0f, // Dist
        Scene_CreateMask(2, SHIP_FLAG_COMPONENT_ID, nationStruct->enemyNationFlag),
        120, // Time
        &Shell_Create,
        false
    };
    Scene_Assign(scene, cruiserID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, cruiserID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, cruiserID, HOVERABLE_COMPONENT_ID, &hoverable);

    Scene_Assign(scene, cruiserID, SHIP_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cruiserID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cruiserID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, cruiserID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return cruiserID;
}
