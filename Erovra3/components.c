#pragma once
#include "scene.h"
#include "components.h"

void Components_Init(struct scene* scene) {
	TRANSFORM_ID = Scene_RegisterComponent(scene, sizeof(Transform));
	SIMPLE_RENDERABLE_ID = Scene_RegisterComponent(scene, sizeof(SimpleRenderable));
	HEALTH_ID = Scene_RegisterComponent(scene, sizeof(Health));
	UNIT_TYPE_ID = Scene_RegisterComponent(scene, sizeof(UnitType));
	CITY_ID = Scene_RegisterComponent(scene, sizeof(City));
}