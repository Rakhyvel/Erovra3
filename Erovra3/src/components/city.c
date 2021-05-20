#pragma once
#include "./city.h"
#include "../components/components.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "./coin.h"

/*
	Takes in a scene, and some information for a city entity. Registers a new 
	entity, assigns components to that entity based on the given information. 
	Returns EntityID of the created city */
EntityID City_Create(struct scene* scene, Vector pos, EntityID nation, bool isCapital)
{
    EntityID cityID = Scene_NewEntity(scene);
    printf("%d\n", cityID);
    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false
    };
    Scene_Assign(scene, cityID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        isCapital ? CAPITAL_TEXTURE_ID : CITY_TEXTURE_ID,
        BUILDING_OUTLINE_TEXTURE_ID,
        BUILDING_SHADOW_TEXTURE_ID,
        false,
        false,
        nation,
        32,
        32,
        36,
        36
    };
    Scene_Assign(scene, cityID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
    };
    Scene_Assign(scene, cityID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_CITY,
        1
    };
    Scene_Assign(scene, cityID, UNIT_COMPONENT_ID, &type);

	Producer producer = {
        6000,
        UnitType_INFANTRY,
        true,
        INVALID_ENTITY_INDEX
    };
    Scene_Assign(scene, cityID, PRODUCER_COMPONENT_ID, &producer);

    ResourceProducer resourceProducer = {
        0.75,
        &Coin_Create
    };
    Scene_Assign(scene, cityID, RESOURCE_PRODUCER_COMPONENT_ID, &resourceProducer);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, cityID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
		FACTORY_BUSY_FOCUSED_GUI
    };
    Scene_Assign(scene, cityID, FOCUSABLE_COMPONENT_ID, &focusable);

    char name[20] = "Name";
    City city = {
        name,
        false
    };
    for (int i = 0; i < 4; i++) {
        city.buildings[i] = INVALID_ENTITY_INDEX;
    }
    Scene_Assign(scene, cityID, CITY_COMPONENT_ID, &city);

    Scene_Assign(scene, cityID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cityID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cityID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, cityID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return cityID;
}