#include "../scenes/match.h"
#include "../textures.h"
#include "./assemblages.h"
#include "./components.h"

EntityID Timberland_Create(struct scene* scene, Vector pos, Nation* nation)
{
    EntityID timberlandID = Scene_NewEntity(scene);

    Sprite sprite = {
        MINE_TEXTURE_ID,
        MINE_OUTLINE_TEXTURE_ID,
        MINE_SHADOW_TEXTURE_ID,
        nation,
        pos,
        (struct vector) { 0.0f, 0.0f },
        0.5f,
        0,
        0,
        0,
        0,
        RenderPriorirty_BUILDING_LAYER,
        32,
        32,
        0,
        false,
        false,
        false,
    };
    Scene_Assign(scene, timberlandID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, timberlandID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_TIMBERLAND,
        0.04f,
        nation->unitCount[UnitType_TIMBERLAND],
        0,
        100,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, timberlandID, UNIT_COMPONENT_ID, &type);

    ResourceProducer resourceProducer = {
        ResourceType_TIMBER,
        0,
        10000,
        &Timber_Create
    };
    Scene_Assign(scene, timberlandID, RESOURCE_PRODUCER_COMPONENT_ID, &resourceProducer);

    Scene_Assign(scene, timberlandID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, timberlandID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, timberlandID, nation->controlFlag, NULL);

    return timberlandID;
}