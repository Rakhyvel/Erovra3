#include "./components.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "./entities.h"

EntityID Airfield_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID airfieldID = Scene_NewEntity(scene);

    Sprite sprite = {
        AIRFIELD_TEXTURE_ID,
        AIRFIELD_OUTLINE_TEXTURE_ID,
        AIRFIELD_SHADOW_TEXTURE_ID,
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
    Scene_Assign(scene, airfieldID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, airfieldID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_AIRFIELD,
        1,
        0,
        0,
        false,
        false,
        BUILDING_FOCUSED_GUI
    };
    Scene_Assign(scene, airfieldID, UNIT_COMPONENT_ID, &type);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, airfieldID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, airfieldID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, airfieldID, nation->controlFlag, NULL);

    return airfieldID;
}
