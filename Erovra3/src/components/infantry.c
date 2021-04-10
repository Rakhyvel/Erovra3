#pragma once 
#include "../textures.h"
#include "components.h"
#include "infantry.h"

/*
	Takes in a scene and some information relevant to infantry entities, registers
	a new entity, assigns components to that infantry entity. Returns the EntityID
	of the infantry */
EntityID Infantry_Create(Scene* scene, Vector pos, EntityID nation)
{
    EntityID infantryID = Scene_NewEntity(scene);
    Transform transform = {
        pos,
        0.5f,
        (struct vector) { 0.0f, 0.0f },
        (struct vector) { 0.0f, 0.0f },
        (struct vector) { 0.0f, 0.0f },
        0,
		0.2f
    };
    Scene_Assign(scene, infantryID, TRANSFORM_COMPONENT_ID, &transform);

    SimpleRenderable render = {
        INFANTRY_TEXTURE_ID,
        GROUND_OUTLINE_TEXTURE_ID,
        false,
        nation,
		32,
		16,
		36,
		20
    };
    Scene_Assign(scene, infantryID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Health health = {
        100.0f,
        0.0f,
        0.0f,
    };
    Scene_Assign(scene, infantryID, HEALTH_COMPONENT_ID, &health);

    Unit type = {
        0,
        0
    };
    Scene_Assign(scene, infantryID, UNIT_COMPONENT_ID, &type);

	Selectable selectable = {
        false,
        false
    };
    Scene_Assign(scene, infantryID, SELECTABLE_COMPONENT_ID, &selectable);

	Scene_Assign(scene, infantryID, GROUND_COMPONENT_ID, NULL);
}
