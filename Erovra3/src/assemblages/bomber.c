#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Bomber_Create(Scene* scene, Vector pos, Nation* nation)
{
    EntityID bomberID = Scene_NewEntity(scene);

    Sprite sprite = {
        BOMBER_TEXTURE_ID,
        BOMBER_OUTLINE_TEXTURE_ID,
        BOMBER_SHADOW_TEXTURE_ID,
        nation,
        pos,
        (struct vector) { 0.0f, 0.0f },
        1.0f,
        0,
        0.56f, // speed
		0,
        0,
        RenderPriorirty_AIR_LAYER,
        84, //42
        54, //41
        0,
        false,
        false,
        false,
    };
    Scene_Assign(scene, bomberID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, bomberID, PLANE_LAYER_COMPONENT_ID, 0);

    Target target = {
        pos,
        pos,
    };
    Scene_Assign(scene, bomberID, TARGET_COMPONENT_ID, &target);

    Patrol patrol = {
        pos,
        pos
    };
    Scene_Assign(scene, bomberID, PATROL_COMPONENT_ID, &patrol);

    Unit type = {
        100.0f,
        0,
        0,
        Scene_CreateMask(scene, 1, AIR_BULLET_COMPONENT_ID),
        false,
        UnitType_BOMBER,
        0.5f, // Defense
        nation->unitCount[UnitType_BOMBER],
        0,
        100,
        false,
        false,
        UNIT_FOCUSED_GUI
    };
    Scene_Assign(scene, bomberID, UNIT_COMPONENT_ID, &type);

    Combatant combatant = {
        50.0f, // Attack amount
        15.0f, // Attack dist
        Scene_CreateMask(scene, 2, SPRITE_COMPONENT_ID, EXPANSION_COMPONENT_ID),
        15, // Attack time (ticks)
        &Bomb_Create,
        true
    };
    Scene_Assign(scene, bomberID, COMBATANT_COMPONENT_ID, &combatant);

    Scene_Assign(scene, bomberID, AIRCRAFT_FLAG_COMPONENT_ID, NULL);
    Scene_Assign(scene, bomberID, nation->controlFlag, NULL);
    return bomberID;
}
