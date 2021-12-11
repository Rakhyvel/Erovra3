#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Battleship_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID battleshipID = Scene_NewEntity(scene);
    Nation* nationStruct = (Nation*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

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
        16,
        46
    };
    Scene_Assign(scene, battleshipID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, battleshipID, SURFACE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, battleshipID, TARGET_COMPONENT_ID, &target);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, battleshipID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_BATTLESHIP,
        2.0f,
        nationStruct->unitCount[UnitType_BATTLESHIP]
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

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, battleshipID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, battleshipID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, battleshipID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, battleshipID, SHIP_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, battleshipID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, battleshipID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return battleshipID;
}
