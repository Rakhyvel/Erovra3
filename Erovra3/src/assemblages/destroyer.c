#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Destroyer_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID destroyerID = Scene_NewEntity(scene);

    Sprite sprite = {
        DESTROYER_TEXTURE_ID,
        SHIP_OUTLINE_TEXTURE_ID,
        SHIP_SHADOW_TEXTURE_ID,
        nation,
        pos,
        (struct vector) { 0.0f, 0.0f },
        0.0f,
        0,
        0.4f, // speed
		0,
        0,
        RenderPriorirty_SURFACE_LAYER,
        16,
        46,
        0,
        false,
        false,
        false,
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
        nation->unitCount[UnitType_DESTROYER],
        0,
        100,
        false,
        false,
        UNIT_FOCUSED_GUI
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

    Morale morale = {
        1.0f
    };
    Scene_Assign(scene, destroyerID, MORALE_COMPONENT_ID, &morale);

    Scene_Assign(scene, destroyerID, SHIP_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, destroyerID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, destroyerID, nation->controlFlag, NULL);
    return destroyerID;
}
