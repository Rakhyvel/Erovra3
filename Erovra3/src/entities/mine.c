#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Mine_Create(struct scene* scene, Vector pos, EntityID nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID mineID = Scene_NewEntity(scene);
    Nation* nationStruct = (Nation*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

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

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, mineID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_MINE,
        0.05f,
        nationStruct->unitCount[UnitType_MINE]
    };
    type.name[0] = 'm';
    type.name[1] = 'i';
    type.name[2] = 'n';
    type.name[3] = 'e';
    Scene_Assign(scene, mineID, UNIT_COMPONENT_ID, &type);

    ResourceProducer resourceProducer = {
        // A good mine is roughly able to support two factories
        (3.0f / Terrain_GetOre(terrain, (int)pos.x, (int)pos.y)),
        &Ore_Create
    };
    Scene_Assign(scene, mineID, RESOURCE_PRODUCER_COMPONENT_ID, &resourceProducer);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, mineID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        BUILDING_FOCUSED_GUI
    };
    Scene_Assign(scene, mineID, FOCUSABLE_COMPONENT_ID, &focusable);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, mineID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, mineID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, mineID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, mineID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return mineID;
}