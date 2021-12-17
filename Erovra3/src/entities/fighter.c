#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Fighter_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID fighterID = Scene_NewEntity(scene);

    Sprite sprite = {
        pos,
        1.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        1.13f, // speed
        false,
		0,
        0,
        FIGHTER_TEXTURE_ID,
        FIGHTER_OUTLINE_TEXTURE_ID,
        FIGHTER_SHADOW_TEXTURE_ID,
        RenderPriorirty_AIR_LAYER,
        false,
        false,
        nation,
        42, //42
        41, //41
        42,
        41
    };
    Scene_Assign(scene, fighterID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, fighterID, PLANE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, fighterID, TARGET_COMPONENT_ID, &target);

    Patrol patrol = {
        pos,
        pos
    };
    Scene_Assign(scene, fighterID, PATROL_COMPONENT_ID, &patrol);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 1, AIR_BULLET_COMPONENT_ID),
        false,
        UnitType_FIGHTER,
        0.5f, // Defense
        nation->unitCount[UnitType_FIGHTER],
        0,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, fighterID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        5.0f, // Attack amount
        4 * 64.0f, // Attack dist
        Scene_CreateMask(scene, 2, SPRITE_COMPONENT_ID, AIRCRAFT_FLAG_COMPONENT_ID),
        15, // Attack time (ticks)
        &AirBullet_Create,
        true
    };
    Scene_Assign(scene, fighterID, COMBATANT_COMPONENT_ID, &combatant);

    Scene_Assign(scene, fighterID, AIRCRAFT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, fighterID, nation->controlFlag, NULL);
    return fighterID;
}
