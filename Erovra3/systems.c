#pragma once
#include "scene.h"
#include "components.h"
#include "systems.h"
#include "gameState.h"
#include "terrain.h"

void System_Transform(struct scene* scene)
{
	const ComponentMask transformMask = Scene_CreateMask(1, TRANSFORM_ID);
	EntityID id;
	for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask))
	{
		Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_ID);

		transform->tar.x = (float)rand() / (float)RAND_MAX * 16 * 64;
		transform->tar.y = (float)rand() / (float)RAND_MAX * 16 * 64;

		transform->vel.x = (transform->tar.x - transform->pos.x) * 0.01;
		transform->vel.y = (transform->tar.y - transform->pos.y) * 0.01;

		transform->pos.x += transform->vel.x;
		transform->pos.y += transform->vel.y;
	}
}

void System_Render(struct scene* scene)
{
	const ComponentMask renderMask = Scene_CreateMask(2, TRANSFORM_ID, SIMPLE_RENDERABLE_ID);
	EntityID id;
	for (id = Scene_Begin(scene, renderMask); Scene_End(scene, id); id = Scene_Next(scene, id, renderMask))
	{
		Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_ID);
		SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_ID);

		SDL_Rect rect = {0, 0, 0, 0};
		terrain_translate(&rect, transform->pos.x, transform->pos.y, 64, 64);
		SDL_RenderFillRect(g->rend, &rect);
	}
}