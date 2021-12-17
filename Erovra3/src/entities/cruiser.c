#pragma once
#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Cruiser_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID cruiserID = Scene_NewEntity(scene);

    Sprite sprite = {
        pos,
        0.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.2f, // speed
        false,
		0,
        0,
        CRUISER_TEXTURE_ID,
        SHIP_OUTLINE_TEXTURE_ID,
        SHIP_SHADOW_TEXTURE_ID,
        RenderPriorirty_SURFACE_LAYER,
        false,
        false,
        nation,
        16,
        46,
        16,
        46
    };
    Scene_Assign(scene, cruiserID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, cruiserID, SURFACE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, cruiserID, TARGET_COMPONENT_ID, &target);
	
    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_CRUISER,
        1.0f,
        nation->unitCount[UnitType_CRUISER],
        0,
        false,
    };
    Scene_Assign(scene, cruiserID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        2.0f, // Attack
        250.0f, // Dist
        Scene_CreateMask(scene, 2, SPRITE_COMPONENT_ID, SHIP_FLAG_COMPONENT_ID),
        120, // Time
        &Shell_Create,
        false
    };
    Scene_Assign(scene, cruiserID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, cruiserID, SELECTABLE_COMPONENT_ID, &selectable);

    Focusable focusable = {
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, cruiserID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, cruiserID, SHIP_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cruiserID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cruiserID, nation->controlFlag, NULL);
    return cruiserID;
}
