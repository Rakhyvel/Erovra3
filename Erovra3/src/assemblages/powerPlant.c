#include "../scenes/match.h"
#include "../textures.h"
#include "./assemblages.h"
#include "./components.h"

EntityID PowerPlant_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID powerPlantID = Scene_NewEntity(scene);

    Sprite sprite = {
        POWERPLANT_TEXTURE_ID,
        POWERPLANT_OUTLINE_TEXTURE_ID,
        POWERPLANT_SHADOW_TEXTURE_ID,
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
    Scene_Assign(scene, powerPlantID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, powerPlantID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_POWERPLANT,
        1,
        nation->unitCount[UnitType_POWERPLANT],
        0,
        100,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, powerPlantID, UNIT_COMPONENT_ID, &type);

    ResourceAccepter accepter;
    memset(&accepter, 0, sizeof(accepter));
    accepter.capacity[ResourceType_COAL] = 1;
    Scene_Assign(scene, powerPlantID, RESOURCE_ACCEPTER_COMPONENT_ID, &accepter);

    ResourceProducer resourceProducer = {
        ResourceType_POWER,
        -1, // Start at -1 so that it doesn't automatically create power (only creates on 0)
        40,
        &Power_Create
    };
    Scene_Assign(scene, powerPlantID, RESOURCE_PRODUCER_COMPONENT_ID, &resourceProducer);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, powerPlantID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, powerPlantID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, powerPlantID, nation->controlFlag, NULL);

    return powerPlantID;
}
