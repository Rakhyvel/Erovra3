#pragma once
#include "scene.h"
#include "components.h"
#include "systems.h"
#include "gameState.h"
#include "terrain.h"
#include "textureManager.h"

void System_Transform(struct scene* scene)
{
	const ComponentMask transformMask = Scene_CreateMask(1, TRANSFORM_COMPONENT_ID);
	EntityID id;
	for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask))
	{
		Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_COMPONENT_ID);

		transform->vel.x = (transform->tar.x - transform->pos.x);
		transform->vel.y = (transform->tar.y - transform->pos.y);
		float mag = sqrtf((transform->vel.x * transform->vel.x) + (transform->vel.y * transform->vel.y));
		transform->vel.x /= mag / 0.2;
		transform->vel.y /= mag / 0.2;

		transform->pos.x += transform->vel.x;
		transform->pos.y += transform->vel.y;
	}
}

void System_Render(struct scene* scene)
{
	const ComponentMask renderMask = Scene_CreateMask(2, TRANSFORM_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID);
	EntityID id;
	for (id = Scene_Begin(scene, renderMask); Scene_End(scene, id); id = Scene_Next(scene, id, renderMask))
	{
		Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_COMPONENT_ID);
		SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);

		SDL_FRect rect = {0, 0, 0, 0};
		terrain_translate(&rect, transform->pos.x, transform->pos.y, 32.0, 32.0);
		if ((float)rand() / (float)RAND_MAX < 0.5)
		{
			Texture_ColorMod(simpleRenderable->sprite, 45, 80, 250);
		}
		else
		{
			Texture_ColorMod(simpleRenderable->sprite, 210, 175, 5);
		}
		Texture_Draw(simpleRenderable->sprite, rect.x, rect.y, rect.w / 32.0, rect.h / 32.0);
	}
}