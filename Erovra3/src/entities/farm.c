#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"
#include <string.h>

EntityID Farm_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID farmID = Scene_NewEntity(scene);

    Sprite sprite = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false,
		0,
        0,
        FARM_TEXTURE_ID,
        FARM_OUTLINE_TEXTURE_ID,
        FARM_SHADOW_TEXTURE_ID,
        RenderPriorirty_BUILDING_LAYER,
        false,
        false,
        nation,
        32,
        32
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
        1,
        nation->unitCount[UnitType_FARM],
        0,
        false,
        false,
        BUILDING_FOCUSED_GUI
    };
    Scene_Assign(scene, farmID, UNIT_COMPONENT_ID, &type);

	ResourceProducer resourceProducer = {
        1.0f,
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
