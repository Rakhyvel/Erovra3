#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Factory_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID foundryID = Scene_NewEntity(scene);

    Sprite sprite = {
        FACTORY_TEXTURE_ID,
        FACTORY_OUTLINE_TEXTURE_ID,
        FACTORY_SHADOW_TEXTURE_ID,
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
    Scene_Assign(scene, foundryID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, foundryID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_FACTORY,
        1,
        nation->unitCount[UnitType_FACTORY],
        0,
        100,
        false,
        false,
        FACTORY_READY_FOCUSED_GUI
    };
    Scene_Assign(scene, foundryID, UNIT_COMPONENT_ID, &type);

    Producer producer = {
        -1,
        -1,
        false,
        FACTORY_READY_FOCUSED_GUI,
        FACTORY_BUSY_FOCUSED_GUI
    };
    Scene_Assign(scene, foundryID, PRODUCER_COMPONENT_ID, &producer);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, foundryID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, foundryID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, foundryID, nation->controlFlag, NULL);

    return foundryID;
}
