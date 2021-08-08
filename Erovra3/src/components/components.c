#pragma once
#include "../components/components.h"
#include "../engine/scene.h"

/*
	Takes in a scene, registers components used in the game for that scene */
void Components_Init(struct scene* scene)
{
    MOTION_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Motion));
    TARGET_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Target));
    PATROL_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Patrol));
    SIMPLE_RENDERABLE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(SimpleRenderable));
    HEALTH_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Health));

    UNIT_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Unit));
    COMBATANT_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Combatant));
    GROUND_UNIT_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    LAND_UNIT_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    INFANTRY_UNIT_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    BUILDING_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    WALL_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    BULLET_ATTACK_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    SHELL_ATTACK_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    SHIP_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    AIRCRAFT_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);

    PROJECTILE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Projectile));
    BULLET_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    AIR_BULLET_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    BOMB_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    SHELL_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Shell));

    CITY_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(City));
    PRODUCER_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Producer));
    EXPANSION_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Expansion));

    RESOURCE_PARTICLE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(ResourceParticle));
    RESOURCE_PRODUCER_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(ResourceProducer));

    NATION_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Nation));
    HOME_NATION_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    ENEMY_NATION_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    PLAYER_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    AI_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);

    HOVERABLE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Hoverable));
    SELECTABLE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Selectable));
    FOCUSABLE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Focusable));
}

