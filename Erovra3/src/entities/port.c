#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Port_Create(struct scene* scene, Vector pos, EntityID nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID portID = Scene_NewEntity(scene);
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
        PORT_TEXTURE_ID,
        PORT_OUTLINE_TEXTURE_ID,
        PORT_SHADOW_TEXTURE_ID,
        RenderPriorirty_BUILDING_LAYER,
        false,
        false,
        nation,
        32,
        32,
        32,
        32
    };
    Scene_Assign(scene, portID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, portID, BUILDING_LAYER_COMPONENT_ID, 0);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, portID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_PORT,
        1,
        nationStruct->unitCount[UnitType_PORT]
    };
    Scene_Assign(scene, portID, UNIT_COMPONENT_ID, &type);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, portID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        PORT_READY_FOCUSED_GUI
    };
    Scene_Assign(scene, portID, FOCUSABLE_COMPONENT_ID, &focusable);

    Producer producer = {
        -1,
        INVALID_ENTITY_INDEX,
        false,
        PORT_READY_FOCUSED_GUI,
        PORT_BUSY_FOCUSED_GUI
    };
    Scene_Assign(scene, portID, PRODUCER_COMPONENT_ID, &producer);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, portID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, portID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, portID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return portID;
}
