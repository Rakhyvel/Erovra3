#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Infantry_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID infantryID = Scene_NewEntity(scene);

    Sprite sprite = {
        INFANTRY_TEXTURE_ID,
        GROUND_OUTLINE_TEXTURE_ID,
        GROUND_SHADOW_TEXTURE_ID,
        nation,
        pos,
        (struct vector) { 0.0f, 0.0f },
        0.5f,
        0,
        0.2f,
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
    Scene_Assign(scene, infantryID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, infantryID, SURFACE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, infantryID, TARGET_COMPONENT_ID, &target);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_INFANTRY,
        1.0f, // 1 damage every 30 ticks -> 300 ticks,
        nation->unitCount[UnitType_INFANTRY],
        0,
        100,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, infantryID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        0.416f,
        68.0f,
        Scene_CreateMask(scene, 1, LAND_UNIT_FLAG_COMPONENT_ID),
        30,
        &Bullet_Create,
        true
    };
    Scene_Assign(scene, infantryID, COMBATANT_COMPONENT_ID, &combatant);

    Scene_Assign(scene, infantryID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, infantryID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, infantryID, BULLET_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, infantryID, nation->controlFlag, NULL);
    return infantryID;
}
