#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Factory_Create(struct scene* scene, Vector pos, EntityID nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID factoryID = Scene_NewEntity(scene);
    Nation* nationStruct = (Nation*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false
    };
    Scene_Assign(scene, factoryID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        FACTORY_TEXTURE_ID,
        FACTORY_OUTLINE_TEXTURE_ID,
        FACTORY_SHADOW_TEXTURE_ID,
        RenderPriorirty_BUILDING_LAYER,
        false,
        false,
        nation,
        32,
        32,
        32,
        32
    };
    Scene_Assign(scene, factoryID, BUILDING_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, factoryID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, factoryID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_FACTORY,
        1,
        nationStruct->unitCount[UnitType_FACTORY]
    };
    Scene_Assign(scene, factoryID, UNIT_COMPONENT_ID, &type);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, factoryID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        FACTORY_READY_FOCUSED_GUI
    };
    Scene_Assign(scene, factoryID, FOCUSABLE_COMPONENT_ID, &focusable);

    Producer producer = {
        -1,
        INVALID_ENTITY_INDEX,
        false,
        FACTORY_READY_FOCUSED_GUI,
        FACTORY_BUSY_FOCUSED_GUI
    };
    Scene_Assign(scene, factoryID, PRODUCER_COMPONENT_ID, &producer);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, factoryID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, factoryID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, factoryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, factoryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return factoryID;
}
