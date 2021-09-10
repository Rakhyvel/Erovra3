#pragma once
#include "attacker.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "bullet.h"
#include "components.h"

EntityID Attacker_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID attackerID = Scene_NewEntity(scene);
    Nation* nationStruct = (Nation*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

    Motion motion = {
        pos,
        1.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.8f, // speed
        false
    };
    Scene_Assign(scene, attackerID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, attackerID, TARGET_COMPONENT_ID, &target);

    Patrol patrol = {
        pos,
        pos
    };
    Scene_Assign(scene, attackerID, PATROL_COMPONENT_ID, &patrol);

    SimpleRenderable render = {
        ATTACKER_TEXTURE_ID,
        ATTACKER_OUTLINE_TEXTURE_ID,
        ATTACKER_SHADOW_TEXTURE_ID,
        RenderPriorirty_AIR_LAYER,
        false,
        false,
        nation,
        58, //42
        48, //41
        58,
        48
    };
    Scene_Assign(scene, attackerID, PLANE_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, attackerID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 1, AIR_BULLET_COMPONENT_ID)
    };
    Scene_Assign(scene, attackerID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_ATTACKER,
        0.5f, // Defense
        nationStruct->unitCount[UnitType_ATTACKER]
    };
    Scene_Assign(scene, attackerID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        0.4f, // Attack amount
        128.0f, // Attack dist
        Scene_CreateMask(scene, 3, MOTION_COMPONENT_ID, GROUND_UNIT_FLAG_COMPONENT_ID, nationStruct->enemyNationFlag),
        15, // Attack time (ticks)
        &Bullet_Create,
        true
    };
    Scene_Assign(scene, attackerID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, attackerID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, attackerID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, attackerID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, attackerID, AIRCRAFT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, attackerID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, attackerID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return attackerID;
}
