#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"
#include <string.h>

EntityID City_Create(struct scene* scene, Vector pos, Nation* nation, char* nameBuffer, bool isCapital)
{
    EntityID cityID = Scene_NewEntity(scene);
    Arraylist_Add(&(nation->cities), &cityID);

    Sprite sprite = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0,
        false,
		0,
        0,
        isCapital ? CAPITAL_TEXTURE_ID : CITY_TEXTURE_ID,
        isCapital ? CAPITAL_OUTLINE_TEXTURE_ID : CITY_OUTLINE_TEXTURE_ID,
        isCapital ? CAPITAL_SHADOW_TEXTURE_ID : CITY_SHADOW_TEXTURE_ID,
        RenderPriorirty_BUILDING_LAYER,
        false,
        false,
        nation,
        32,
        32,
        32,
        32
    };
    Scene_Assign(scene, cityID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, cityID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 2, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID),
        false,
        UnitType_CITY,
        1.0f,
        0,
        0,
        false,
        false,
		UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, cityID, UNIT_COMPONENT_ID, &type);

    ResourceProducer resourceProducer = {
        1.0f,
        &Coin_Create
    };
    Scene_Assign(scene, cityID, RESOURCE_PRODUCER_COMPONENT_ID, &resourceProducer);

    City city;
    strcpy_s(city.name, 20, nameBuffer);
    city.isCapital = isCapital;
    for (int i = 0; i < 4; i++) {
        city.expansions[i] = INVALID_ENTITY_INDEX;
    }
    Scene_Assign(scene, cityID, CITY_COMPONENT_ID, &city);

    Scene_Assign(scene, cityID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cityID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cityID, nation->controlFlag, NULL);

    return cityID;
}