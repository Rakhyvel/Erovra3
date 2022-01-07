#include "../scenes/match.h"
#include "../textures.h"
#include "./assemblages.h"
#include "./components.h"
#include <string.h>

EntityID Farm_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID farmID = Scene_NewEntity(scene);

    Sprite sprite = {
        FARM_TEXTURE_ID,
        FARM_OUTLINE_TEXTURE_ID,
        FARM_SHADOW_TEXTURE_ID,
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
    Scene_Assign(scene, farmID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, farmID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_FARM,
        0.04f,
        nation->unitCount[UnitType_FARM],
        0,
        100,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, farmID, UNIT_COMPONENT_ID, &type);

    ResourceProducer resourceProducer = {
        ResourceType_FOOD,
        0,
        ticksPerLabor * 0.1f,
        &Food_Create
    };
    Scene_Assign(scene, farmID, RESOURCE_PRODUCER_COMPONENT_ID, &resourceProducer);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, farmID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, farmID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, farmID, nation->controlFlag, NULL);

    return farmID;
}
