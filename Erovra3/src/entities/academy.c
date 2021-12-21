#include "./components.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "./entities.h"

EntityID Academy_Create(struct scene* scene, Vector pos, Nation* nation, EntityID homeCity, CardinalDirection dir)
{
    EntityID academyID = Scene_NewEntity(scene);

    Sprite sprite = {
        ACADEMY_TEXTURE_ID,
        ACADEMY_OUTLINE_TEXTURE_ID,
        ACADEMY_SHADOW_TEXTURE_ID,
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
    Scene_Assign(scene, academyID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, academyID, BUILDING_LAYER_COMPONENT_ID, 0);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID),
        false,
        UnitType_ACADEMY,
        1,
        0,
        0,
        false,
        false,
        ACADEMY_READY_FOCUSED_GUI
    };
    Scene_Assign(scene, academyID, UNIT_COMPONENT_ID, &type);

    Producer producer = {
        -1,
        -1,
        false,
        ACADEMY_READY_FOCUSED_GUI,
        ACADEMY_BUSY_FOCUSED_GUI
    };
    Scene_Assign(scene, academyID, PRODUCER_COMPONENT_ID, &producer);

    Expansion expansion = {
        homeCity,
        dir
    };
    Scene_Assign(scene, academyID, EXPANSION_COMPONENT_ID, &expansion);

    Scene_Assign(scene, academyID, BUILDING_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, academyID, nation->controlFlag, NULL);

    return academyID;
}
