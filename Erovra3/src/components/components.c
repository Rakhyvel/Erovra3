#pragma once
#include "../engine/scene.h"
#include "../components/components.h"

void Components_Init(struct scene* scene) {
	TRANSFORM_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Transform));
	SIMPLE_RENDERABLE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(SimpleRenderable));
	HEALTH_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(Health));
	UNIT_TYPE_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(UnitType));
	CITY_COMPONENT_ID = Scene_RegisterComponent(scene, sizeof(City));
}