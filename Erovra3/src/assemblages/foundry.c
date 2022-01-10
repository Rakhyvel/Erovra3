#include "../scenes/match.h"
#include "../textures.h"
#include "./assemblages.h"
#include "./components.h"

EntityID Foundry_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID foundryID = Scene_NewEntity(scene);

    Sprite sprite = {
        FOUNDRY_TEXTURE_ID,
        FOUNDRY_OUTLINE_TEXTURE_ID,
        FOUNDRY_SHADOW_TEXTURE_ID,
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
        UnitType_FOUNDRY,
        1,
        nation->unitCount[UnitType_FOUNDRY],
        0,
        100,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, foundryID, UNIT_COMPONENT_ID, &type);

    ResourceAccepter accepter;
    memset(&accepter, 0, sizeof(accepter));
    accepter.capacity[ResourceType_COAL] = 1; // Keep capacity at 1 so that mines don't send resources to the foundry over and over again
    accepter.capacity[ResourceType_ORE] = 1;
    accepter.capacity[ResourceType_POWER] = 1;
    Scene_Assign(scene, foundryID, RESOURCE_ACCEPTER_COMPONENT_ID, &accepter);

    ResourceProducer resourceProducer = {
        ResourceType_METAL,
        -1, // Start at -1 so that it doesn't automatically create metal (only creates on 0)
        480,
        &Metal_Create,
        720
    };
    Scene_Assign(scene, foundryID, RESOURCE_PRODUCER_COMPONENT_ID, &resourceProducer);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, foundryID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, foundryID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, foundryID, nation->controlFlag, NULL);

    return foundryID;
}
