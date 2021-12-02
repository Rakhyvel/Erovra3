#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"
#include <string.h>

EntityID Engineer_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID engineerID = Scene_NewEntity(scene);
    Nation* nationStruct = (Nation*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.2f,
        false
    };
    Scene_Assign(scene, engineerID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, engineerID, TARGET_COMPONENT_ID, &target);

    SimpleRenderable render = {
        ENGINEER_TEXTURE_ID,
        GROUND_OUTLINE_TEXTURE_ID,
        GROUND_SHADOW_TEXTURE_ID,
        RenderPriorirty_SURFACE_LAYER,
        false,
        false,
        nation,
        32,
        16,
        36,
        20
    };
    Scene_Assign(scene, engineerID, SURFACE_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, engineerID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, engineerID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_ENGINEER,
        0.0005f, // 1-shot kill,
        nationStruct->unitCount[UnitType_ENGINEER]
    };
    Scene_Assign(scene, engineerID, UNIT_COMPONENT_ID, &type);
    strncpy_s(type.name, 32, "Engineer", 32);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, engineerID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, engineerID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        ENGINEER_FOCUSED_GUI
    };
    Scene_Assign(scene, engineerID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, engineerID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, engineerID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, engineerID, BULLET_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, engineerID, ENGINEER_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, engineerID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return engineerID;
}
