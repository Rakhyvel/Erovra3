#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Wall_Create(Scene* scene, Vector pos, float angle, Nation* nation)
{
    EntityID wallID = Scene_NewEntity(scene);

    Sprite sprite = {
        WALL_TEXTURE_ID,
        WALL_OUTLINE_TEXTURE_ID,
        WALL_SHADOW_TEXTURE_ID,
        nation,
        pos,
        (struct vector) { 0.0f, 0.0f },
        0.5f,
        angle,
        0.0f,
		0,
        0,
        RenderPriorirty_BUILDING_LAYER,
        64,
        64,
        0,
        false,
        false,
        false,
    };
    Scene_Assign(scene, wallID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, wallID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_WALL,
        1.0f,
        nation->unitCount[UnitType_WALL],
        0,
		100,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, wallID, UNIT_COMPONENT_ID, &type);

    Scene_Assign(scene, wallID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, wallID, WALL_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, wallID, nation->controlFlag, NULL);
    return wallID;
}
