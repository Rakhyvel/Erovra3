#pragma once
#include "../components/components.h"
#include "../engine/scene.h"

/*
	Takes in a scene, registers components used in the game for that scene */
void Components_Init(struct scene* scene)
{
    MOTION_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Motion));
    TARGET_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Target));
	SIMPLE_RENDERABLE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(SimpleRenderable));
    HEALTH_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Health));

    UNIT_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Unit));
    GROUND_UNIT_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    LAND_UNIT_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);

    PROJECTILE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Projectile));

    CITY_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(City));
    NATION_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Nation));
    HOME_NATION_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    ENEMY_NATION_FLAG_COMPONENT_ID = Scene_RegisterComponent(scene, 0);
    SELECTABLE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Selectable));
}