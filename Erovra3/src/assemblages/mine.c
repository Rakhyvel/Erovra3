#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Mine_Create(struct scene* scene, Vector pos, Nation* nation)
{
    EntityID mineID = Scene_NewEntity(scene);

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
    Scene_Assign(scene, mineID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, mineID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_MINE,
        0.04f, // Defense
        nation->unitCount[UnitType_MINE],
        0,
        100,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, mineID, UNIT_COMPONENT_ID, &type);

    ResourceProducer resourceProducer = {
        Terrain_GetOre(terrain, (int)pos.x, (int)pos.y) > 0 ? ResourceType_ORE : ResourceType_COAL,
		0,
		10000,
        Terrain_GetOre(terrain, (int)pos.x, (int)pos.y) > 0 ? &Ore_Create : &Coal_Create
    };
    Scene_Assign(scene, mineID, RESOURCE_PRODUCER_COMPONENT_ID, &resourceProducer);

    Scene_Assign(scene, mineID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, mineID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, mineID, nation->controlFlag, NULL);

    return mineID;
}