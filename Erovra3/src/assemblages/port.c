#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Port_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID portID = Scene_NewEntity(scene);

    Sprite sprite = {
        PORT_TEXTURE_ID,
        PORT_OUTLINE_TEXTURE_ID,
        PORT_SHADOW_TEXTURE_ID,
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
    Scene_Assign(scene, portID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, portID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_PORT,
        1,
        nation->unitCount[UnitType_PORT],
        0,
        100,
        false,
        false,
        PORT_READY_FOCUSED_GUI
    };
    Scene_Assign(scene, portID, UNIT_COMPONENT_ID, &type);

    Producer producer = {
        -1,
        -1,
        false,
        PORT_READY_FOCUSED_GUI,
        PORT_BUSY_FOCUSED_GUI,
        720
    };
    Scene_Assign(scene, portID, PRODUCER_COMPONENT_ID, &producer);

    ResourceAccepter accepter;
    memset(&accepter, 0, sizeof(accepter));
    accepter.capacity[ResourceType_POWER] = 1;
    Scene_Assign(scene, portID, RESOURCE_ACCEPTER_COMPONENT_ID, &accepter);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, portID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, portID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, portID, nation->controlFlag, NULL);

    return portID;
}
