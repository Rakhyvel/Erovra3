#pragma once
#include "systems.h"
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../terrain.h"

void System_Transform(struct scene* scene)
{
    const ComponentMask transformMask = Scene_CreateMask(1, TRANSFORM_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
        Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_COMPONENT_ID);

        transform->vel.x = (transform->tar.x - transform->pos.x);
        transform->vel.y = (transform->tar.y - transform->pos.y);
        float mag = sqrtf((transform->vel.x * transform->vel.x) + (transform->vel.y * transform->vel.y));
        if (mag > 0.1) {
            transform->vel.x /= mag / 0.2;
            transform->vel.y /= mag / 0.2;
        }

        transform->pos.x += transform->vel.x;
        transform->pos.y += transform->vel.y;
    }
}

void System_Render(struct scene* scene)
{
    const ComponentMask renderMask = Scene_CreateMask(2, TRANSFORM_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, renderMask); Scene_End(scene, id); id = Scene_Next(scene, id, renderMask)) {
        Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);

        SDL_FRect rect = { 0, 0, 0, 0 };
        terrain_translate(&rect, transform->pos.x, transform->pos.y, 32.0, 32.0);
        Texture_ColorMod(simpleRenderable->sprite, 250, 100, 80);
        Texture_Draw(simpleRenderable->sprite, rect.x, rect.y, rect.w / 32.0, rect.h / 32.0);
    }
}