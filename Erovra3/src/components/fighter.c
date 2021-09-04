#pragma once
#include "fighter.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "bullet.h"
#include "components.h"

/*
	Creates a fighter plane entity */
EntityID Fighter_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID fighterID = Scene_NewEntity(scene);
    Nation* nationStruct = (Nation*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

    Motion motion = {
        pos,
        1.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        1.13f, // speed
        false
    };
    Scene_Assign(scene, fighterID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, fighterID, TARGET_COMPONENT_ID, &target);

    Patrol patrol = {
        pos,
        pos
    };
    Scene_Assign(scene, fighterID, PATROL_COMPONENT_ID, &patrol);

    SimpleRenderable render = {
        FIGHTER_TEXTURE_ID,
        FIGHTER_OUTLINE_TEXTURE_ID,
        FIGHTER_SHADOW_TEXTURE_ID,
        RenderPriorirty_AIR_LAYER,
        false,
        false,
        nation,
        42,//42
        41, //41
        42,
        41
    };
    Scene_Assign(scene, fighterID, PLANE_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, fighterID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 1, AIR_BULLET_COMPONENT_ID)
    };
    Scene_Assign(scene, fighterID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_FIGHTER,
        0.5f, // Defense
        nationStruct->unitCount[UnitType_FIGHTER]
    };
    Scene_Assign(scene, fighterID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        5.0f, // Attack amount
        4 * 64.0f, // Attack dist
        Scene_CreateMask(scene, 3, MOTION_COMPONENT_ID, AIRCRAFT_FLAG_COMPONENT_ID, nationStruct->enemyNationFlag),
        15, // Attack time (ticks)
        &AirBullet_Create,
        true
    };
    Scene_Assign(scene, fighterID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, fighterID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, fighterID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, fighterID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, fighterID, AIRCRAFT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, fighterID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, fighterID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return fighterID;
}
