#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"
#include <string.h>

EntityID Engineer_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID engineerID = Scene_NewEntity(scene);

    Sprite sprite = {
        ENGINEER_TEXTURE_ID,
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
    Scene_Assign(scene, engineerID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, engineerID, SURFACE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, engineerID, TARGET_COMPONENT_ID, &target);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_ENGINEER,
        0.0005f, // 1-shot kill,
        nation->unitCount[UnitType_ENGINEER],
        0,
        false,
        false,
        ENGINEER_FOCUSED_GUI
    };
    Scene_Assign(scene, engineerID, UNIT_COMPONENT_ID, &type);

    Scene_Assign(scene, engineerID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, engineerID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, engineerID, BULLET_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, engineerID, ENGINEER_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, engineerID, nation->controlFlag, NULL);
    return engineerID;
}
