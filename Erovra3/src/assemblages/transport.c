#include "../scenes/match.h"
#include "../textures.h"
#include "./assemblages.h"
#include "./components.h"

EntityID Transport_Create(struct scene* scene, Vector pos, Nation* nation)
{
    EntityID transportID = Scene_NewEntity(scene);

    Sprite sprite = {
        DESTROYER_TEXTURE_ID,
        SHIP_OUTLINE_TEXTURE_ID,
        SHIP_SHADOW_TEXTURE_ID,
        nation,
        pos,
        (struct vector) { 0.0f, 0.0f },
        0.0f,
        0,
        0.4f, // speed
        0,
        0,
        RenderPriorirty_SURFACE_LAYER,
        16,
        24,
        0,
        false,
        false,
        false,
    };
    Scene_Assign(scene, transportID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, transportID, SURFACE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, transportID, TARGET_COMPONENT_ID, &target);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_TRANSPORT,
        0.5f, // Defense
        nation->unitCount[UnitType_TRANSPORT],
        0,
        100,
        false,
        false,
        TRANSPORT_FOCUSED_GUI
    };
    Scene_Assign(scene, transportID, UNIT_COMPONENT_ID, &type);

    Boardable boardable = {
        0,
        { INVALID_ENTITY_INDEX, INVALID_ENTITY_INDEX, INVALID_ENTITY_INDEX }
    };
    Scene_Assign(scene, transportID, BOARDABLE_COMPONENT_ID, &boardable);

    Scene_Assign(scene, transportID, SHIP_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, transportID, nation->controlFlag, NULL);
    return transportID;
}
