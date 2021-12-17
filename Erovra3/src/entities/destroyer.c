#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Destroyer_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID destroyerID = Scene_NewEntity(scene);

    Sprite sprite = {
        pos,
        0.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.4f, // speed
        false,
		0,
        0,
        DESTROYER_TEXTURE_ID,
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
    Scene_Assign(scene, destroyerID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, destroyerID, SURFACE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, destroyerID, TARGET_COMPONENT_ID, &target);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_DESTROYER,
        0.5f, // Defense
        nation->unitCount[UnitType_DESTROYER]
    };
    Scene_Assign(scene, destroyerID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        1.0f, // Attack
        125.0f, // Dist
        Scene_CreateMask(scene, 2, SPRITE_COMPONENT_ID, SHIP_FLAG_COMPONENT_ID),
        120, // Time
        &Shell_Create,
        false
    };
    Scene_Assign(scene, destroyerID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, destroyerID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, destroyerID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, destroyerID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, destroyerID, SHIP_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, destroyerID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, destroyerID, nation->controlFlag, NULL);
    return destroyerID;
}
