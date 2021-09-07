#pragma once
#include "bomber.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "bomb.h"
#include "components.h"

EntityID Bomber_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID bomberID = Scene_NewEntity(scene);
    Nation* nationStruct = (Nation*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

    Motion motion = {
        pos,
        1.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.56f, // speed
        false
    };
    Scene_Assign(scene, bomberID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, bomberID, TARGET_COMPONENT_ID, &target);

    Patrol patrol = {
        pos,
        pos
    };
    Scene_Assign(scene, bomberID, PATROL_COMPONENT_ID, &patrol);

    SimpleRenderable render = {
        BOMBER_TEXTURE_ID,
        BOMBER_OUTLINE_TEXTURE_ID,
        BOMBER_SHADOW_TEXTURE_ID,
        RenderPriorirty_AIR_LAYER,
        false,
        false,
        nation,
        84, //42
        54, //41
        84,
        54
    };
    Scene_Assign(scene, bomberID, PLANE_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, bomberID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 1, AIR_BULLET_COMPONENT_ID)
    };
    Scene_Assign(scene, bomberID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_BOMBER,
        0.5f, // Defense
        nationStruct->unitCount[UnitType_BOMBER] 
    };
    Scene_Assign(scene, bomberID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        50.0f, // Attack amount
        15.0f, // Attack dist
        Scene_CreateMask(scene, 3, MOTION_COMPONENT_ID, EXPANSION_COMPONENT_ID, nationStruct->enemyNationFlag),
        15, // Attack time (ticks)
        &Bomb_Create,
        true
    };
    Scene_Assign(scene, bomberID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, bomberID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, bomberID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, bomberID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, bomberID, AIRCRAFT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, bomberID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, bomberID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return bomberID;
}
