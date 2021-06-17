#pragma once
#include "../scenes/match.h"
#include "../textures.h"
#include "components.h"
#include "bullet.h"

/*
	Creates a cavalry entity */
EntityID Cavalry_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID cavalryID = Scene_NewEntity(scene);
    Nation* nationStruct = (Unit*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.4f,
        false
    };
    Scene_Assign(scene, cavalryID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, cavalryID, TARGET_COMPONENT_ID, &target);

    SimpleRenderable render = {
        CAVALRY_TEXTURE_ID,
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
    Scene_Assign(scene, cavalryID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
    };
    Scene_Assign(scene, cavalryID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_CAVALRY,
        0.5f
    };
    Scene_Assign(scene, cavalryID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        0.5f,
        68.0f,
        Scene_CreateMask(2, LAND_UNIT_FLAG_COMPONENT_ID, nationStruct->enemyNationFlag),
        30,
        &Bullet_Create,
		true
    };
    Scene_Assign(scene, cavalryID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, cavalryID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, cavalryID, HOVERABLE_COMPONENT_ID, &hoverable);

    Scene_Assign(scene, cavalryID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cavalryID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cavalryID, BULLET_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cavalryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, cavalryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return cavalryID;
}
