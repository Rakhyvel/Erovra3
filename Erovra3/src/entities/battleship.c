#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Battleship_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID battleshipID = Scene_NewEntity(scene);

    Sprite sprite = {
        pos,
        0.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.1f,
        false,
		0,
        0,
        BATTLESHIP_TEXTURE_ID,
        SHIP_OUTLINE_TEXTURE_ID,
        SHIP_SHADOW_TEXTURE_ID,
        RenderPriorirty_SURFACE_LAYER,
        false,
        false,
        nation,
        16,
        46,
    };
    Scene_Assign(scene, battleshipID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, battleshipID, SURFACE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, battleshipID, TARGET_COMPONENT_ID, &target);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_BATTLESHIP,
        2.0f,
        nation->unitCount[UnitType_BATTLESHIP],
        0,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, battleshipID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        4.0f,
        250.0f,
        Scene_CreateMask(scene, 2, SPRITE_COMPONENT_ID, SHIP_FLAG_COMPONENT_ID),
        120,
        &Shell_Create,
        false
    };
    Scene_Assign(scene, battleshipID, COMBATANT_COMPONENT_ID, &combatant);

    Scene_Assign(scene, battleshipID, SHIP_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, battleshipID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, battleshipID, nation->controlFlag, NULL);
    return battleshipID;
}
