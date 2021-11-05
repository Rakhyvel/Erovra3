#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Cavalry_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID cavalryID = Scene_NewEntity(scene);
    Nation* nationStruct = (Nation*)Scene_GetComponent(scene, nation, NATION_COMPONENT_ID);

    Motion motion = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.4f,
        false
    };
    Scene_Assign(scene, cavalryID, MOTION_COMPONENT_ID, &motion);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, cavalryID, TARGET_COMPONENT_ID, &target);

    SimpleRenderable render = {
        CAVALRY_TEXTURE_ID,
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
    Scene_Assign(scene, cavalryID, SURFACE_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, cavalryID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 3, BULLET_COMPONENT_ID, SHELL_COMPONENT_ID, BOMB_COMPONENT_ID)
    };
    Scene_Assign(scene, cavalryID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        UnitType_CAVALRY,
        1.0f,
        nationStruct->unitCount[UnitType_CAVALRY]
    };
    Scene_Assign(scene, cavalryID, UNIT_COMPONENT_ID, &type);

    // -0.25 a second -> 5 * 400 seconds of being alive

    Combatant combatant = {
        0.2f,
        68.0f,
        Scene_CreateMask(scene, 3, MOTION_COMPONENT_ID, LAND_UNIT_FLAG_COMPONENT_ID, nationStruct->enemyNationFlag),
        30,
        &Bullet_Create,
        true
    };
    Scene_Assign(scene, cavalryID, COMBATANT_COMPONENT_ID, &combatant);

    Selectable selectable = {
        false,
    };
    Scene_Assign(scene, cavalryID, SELECTABLE_COMPONENT_ID, &selectable);

    Hoverable hoverable = {
        false,
    };
    Scene_Assign(scene, cavalryID, HOVERABLE_COMPONENT_ID, &hoverable);

    Focusable focusable = {
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, cavalryID, FOCUSABLE_COMPONENT_ID, &focusable);

    Scene_Assign(scene, cavalryID, LAND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cavalryID, GROUND_UNIT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cavalryID, BULLET_ATTACK_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, cavalryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, cavalryID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return cavalryID;
}