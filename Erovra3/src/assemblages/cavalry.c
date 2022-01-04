#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Cavalry_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID cavalryID = Scene_NewEntity(scene);

    Sprite sprite = {
        CAVALRY_TEXTURE_ID,
        GROUND_OUTLINE_TEXTURE_ID,
        GROUND_SHADOW_TEXTURE_ID,
        nation,
        pos,
        (struct vector) { 0.0f, 0.0f },
        0.5f,
        0,
        0.4f,
		0,
        0,
        RenderPriorirty_SURFACE_LAYER,
        36,
        20,
        0,
		false,
        false,
        false,
    };
    Scene_Assign(scene, cavalryID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, cavalryID, SURFACE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, cavalryID, TARGET_COMPONENT_ID, &target);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_CAVALRY,
        1.0f,
        nation->unitCount[UnitType_CAVALRY],
        0,
        100,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, cavalryID, UNIT_COMPONENT_ID, &type);

    // -0.25 a second -> 5 * 400 seconds of being alive

    Combatant combatant = {
        0.416f,
        68.0f,
        Scene_CreateMask(scene, 2, SPRITE_COMPONENT_ID, LAND_UNIT_FLAG_COMPONENT_ID),
        30,
        &Bullet_Create,
        true
    };
    Scene_Assign(scene, cavalryID, COMBATANT_COMPONENT_ID, &combatant);

    Morale morale = {
        1.0f
    };
    Scene_Assign(scene, cavalryID, MORALE_COMPONENT_ID, &morale);

    Scene_Assign(scene, cavalryID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cavalryID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cavalryID, BULLET_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cavalryID, nation->controlFlag, NULL);
    return cavalryID;
}
