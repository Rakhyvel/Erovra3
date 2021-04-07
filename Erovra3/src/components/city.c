#pragma once
#include "./city.h"

#include "../components/components.h"
#include "../textures.h"

EntityID City_Create(struct scene* scene, Vector pos, bool isCapital)
{
    EntityID cityID = Scene_NewEntity(scene);
    Transform transform = {
		pos,
        0.0f,
        (struct vector) { 0.0f, 0.0f },
        (struct vector) { 0.0f, 0.0f },
		pos,
        0.0f
    };
    Scene_Assign(scene, cityID, TRANSFORM_COMPONENT_ID, &transform);

    SimpleRenderable render = {
        isCapital ? CAPITAL_TEXTURE_ID : CITY_TEXTURE_ID,
        BUILDING_OUTLINE_TEXTURE_ID,
        isCapital
    };
    Scene_Assign(scene, cityID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
    };
    Scene_Assign(scene, cityID, HEALTH_COMPONENT_ID, &health);

    UnitType type = {
        0,
        0,
        0
    };
    Scene_Assign(scene, cityID, UNIT_TYPE_COMPONENT_ID, &type);

    char name[20] = "Name";
    City city = {
        name,
        false,
        0
    };
    Scene_Assign(scene, cityID, CITY_COMPONENT_ID, &city);
}