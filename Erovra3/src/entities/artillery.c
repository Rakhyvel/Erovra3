#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Artillery_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID artilleryID = Scene_NewEntity(scene);

    Sprite sprite = {
        ARTILLERY_TEXTURE_ID,
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
    Scene_Assign(scene, artilleryID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, artilleryID, SURFACE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, artilleryID, TARGET_COMPONENT_ID, &target);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_ARTILLERY,
        1.0f,
        nation->unitCount[UnitType_ARTILLERY],
        0,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, artilleryID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        0.8f,
        100.0f,
        Scene_CreateMask(scene, 2, SPRITE_COMPONENT_ID, LAND_UNIT_FLAG_COMPONENT_ID),
        120,
        &Shell_Create,
        true
    };
    Scene_Assign(scene, artilleryID, COMBATANT_COMPONENT_ID, &combatant);

    Scene_Assign(scene, artilleryID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, artilleryID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, artilleryID, SHELL_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, artilleryID, nation->controlFlag, NULL);
    return artilleryID;
}
