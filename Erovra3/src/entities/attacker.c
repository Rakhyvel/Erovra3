#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Attacker_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID attackerID = Scene_NewEntity(scene);

    Sprite sprite = {
        pos,
        1.0f,
        (struct vector) { 0.0f, 0.0f },
        0,
        0.8f, // speed
        false,
		0,
        0,
        ATTACKER_TEXTURE_ID,
        ATTACKER_OUTLINE_TEXTURE_ID,
        ATTACKER_SHADOW_TEXTURE_ID,
        RenderPriorirty_AIR_LAYER,
        false,
        false,
        nation,
        58, //42
        48, //41
    };
    Scene_Assign(scene, attackerID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, attackerID, PLANE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, attackerID, TARGET_COMPONENT_ID, &target);

    Patrol patrol = {
        pos,
        pos
    };
    Scene_Assign(scene, attackerID, PATROL_COMPONENT_ID, &patrol);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 1, AIR_BULLET_COMPONENT_ID),
        false,
        UnitType_ATTACKER,
        0.5f, // Defense
        nation->unitCount[UnitType_ATTACKER],
        0,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, attackerID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        0.2f, // Attack amount
        128.0f, // Attack dist
        Scene_CreateMask(scene, 2, SPRITE_COMPONENT_ID, GROUND_UNIT_FLAG_COMPONENT_ID),
        15, // Attack time (ticks)
        &Bullet_Create,
        true
    };
    Scene_Assign(scene, attackerID, COMBATANT_COMPONENT_ID, &combatant);

    Scene_Assign(scene, attackerID, AIRCRAFT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, attackerID, nation->controlFlag, NULL);
    return attackerID;
}
