#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Mine_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID mineID = Scene_NewEntity(scene);

    Sprite sprite = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false,
		0,
		0,
        MINE_TEXTURE_ID,
        MINE_OUTLINE_TEXTURE_ID,
        MINE_SHADOW_TEXTURE_ID,
        RenderPriorirty_BUILDING_LAYER,
        false,
        false,
        nation,
        32,
        32,
        32,
        32
    };
    Scene_Assign(scene, mineID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, mineID, BUILDING_LAYER_COMPONENT_ID, 0);

    Target target = {
        (struct vector) { 0.0f, 0.0f },
        pos,
    };
    Scene_Assign(scene, mineID, TARGET_COMPONENT_ID, &target);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_MINE,
        0.05f,
        nation->unitCount[UnitType_MINE],
        0,
        false,
        false,
        BUILDING_FOCUSED_GUI
    };
    Scene_Assign(scene, mineID, UNIT_COMPONENT_ID, &type);

    ResourceProducer resourceProducer = {
        // A good mine is roughly able to support two factories
        (3.0f / Terrain_GetOre(terrain, (int)pos.x, (int)pos.y)),
        &Ore_Create
    };
    Scene_Assign(scene, mineID, RESOURCE_PRODUCER_COMPONENT_ID, &resourceProducer);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, mineID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, mineID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, mineID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, mineID, nation->controlFlag, NULL);

    return mineID;
}