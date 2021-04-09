#pragma once
#include "systems.h"
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../terrain.h"

/*
	Takes in a scene, iterates through all entities that have a transform
	component. Updates the velocity based on target and position, and postion
	based on target. */
void System_Transform(struct terrain* terrain, struct scene* scene)
{
    const ComponentMask transformMask = Scene_CreateMask(1, TRANSFORM_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
        Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_COMPONENT_ID);

        // Calculate if pointing in direction of lookat vector
        Vector displacement = Vector_Sub(&transform->pos, &transform->lookat);
        float tempAngle = Vector_Angle(&displacement); // atan2
        if (fabs(transform->angle - tempAngle) < 3.1415926) {
            // Can be compared directly
        } else if (transform->angle < tempAngle) {
            transform->angle += 2 * 3.1415926;
        } else if (transform->angle > tempAngle) {
            transform->angle -= 2 * 3.1415926;
		}
        float diff = fabs(transform->angle - tempAngle);

        if (diff > transform->speed / 18.0f) {
            // Not looking in direction, turn
            if (transform->angle > tempAngle) {
                transform->angle -= transform->speed / 18.0f;
            } else {
                transform->angle += transform->speed / 18.0f;
            }
        } else if (Vector_Dist(&transform->tar, &transform->pos) > transform->speed) {
            // Looking in direction, not at target, move
            transform->vel.x = (transform->tar.x - transform->pos.x);
            transform->vel.y = (transform->tar.y - transform->pos.y);
            float mag = sqrtf((transform->vel.x * transform->vel.x) + (transform->vel.y * transform->vel.y));
            if (mag > 0.1) {
                transform->vel.x /= mag / transform->speed;
                transform->vel.y /= mag / transform->speed;
            }

            displacement = Vector_Add(&(transform->pos), &(transform->vel));
            float height = terrain_getHeight(terrain, (int)displacement.x, (int)displacement.y);
            if (height > transform->z && height < transform->z + 0.5f) {
                Vector_Copy(&transform->pos, &displacement);
            }
        }
    }
}

static bool targeted = false;
static int taskForce = -1;
void System_ChangeTaskForce(struct scene* scene)
{
    int newTaskForce = -1;
    for (int i = 0; i < 10; i++) {
        if (g->keys[i + SDL_SCANCODE_1]) {
            newTaskForce = i;
        }
    }
    if (newTaskForce == -1) {
        return;
    }
    const ComponentMask transformMask = Scene_CreateMask(1, SELECTABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
        Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
        selectable->taskForce = newTaskForce;
    }
}

void System_SetTarget(struct scene* scene)
{
    if (g->ctrl) {
        return;
    }
    const ComponentMask transformMask = Scene_CreateMask(2, SELECTABLE_COMPONENT_ID, TRANSFORM_COMPONENT_ID);
    EntityID id;
    targeted = false;
    for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
        Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_COMPONENT_ID);
        Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
        if (g->mouseLeftUp && selectable->selected) {
            Vector mouse = Terrain_MousePos();
            Vector_Copy(&transform->tar, &mouse);
            Vector_Copy(&transform->lookat, &mouse);
            Vector displacement = Vector_Sub(&transform->tar, &transform->pos);
            selectable->selected = false;
            targeted = true;
        }
    }
}

/*
	 Iterates through all entities with selectable, transform, and simple 
	 renderable components. Detects if they are hovered, then if they should
	 become selected, or if they should have their targets updated. */
void System_Select(struct scene* scene)
{
    if (targeted) {
        return;
    }
    const ComponentMask transformMask = Scene_CreateMask(3, SELECTABLE_COMPONENT_ID, TRANSFORM_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID);
    EntityID id;
    taskForce = -1;
    for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
        Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_COMPONENT_ID);
        Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);

        Vector mouse = Terrain_MousePos();
        float dx = transform->pos.x - mouse.x;
        float dy = mouse.y - transform->pos.y;

        float sin = sinf(transform->angle);
        float cos = cosf(transform->angle);

        bool checkLR = fabs(cos * dx + sin * dy) <= simpleRenderable->width / 2;
        bool checkTB = fabs(sin * dx - cos * dy) <= simpleRenderable->height / 2;

        selectable->isHovered = checkLR && checkTB;

        simpleRenderable->showOutline = selectable->isHovered || selectable->selected;
        if (g->mouseLeftUp && selectable->isHovered && !selectable->selected || (selectable->taskForce == taskForce)) {
            selectable->selected = true;
            if (g->ctrl && g->shift) {
                taskForce = selectable->taskForce;
                continue;
            } else {
                return;
            }
        }
    }
}

/*
	Takes in a scene, iterates through all entities with SimpleRenderable and 
	Transform components. Translates texture based on Terrain's offset and zoom,
	colorizes based on the nation color, and renders texture to screen. */
void System_Render(struct scene* scene)
{
    const ComponentMask renderMask = Scene_CreateMask(2, TRANSFORM_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, renderMask); Scene_End(scene, id); id = Scene_Next(scene, id, renderMask)) {
        Transform* transform = (Transform*)Scene_GetComponent(scene, id, TRANSFORM_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        SDL_FRect rect = { 0, 0, 0, 0 };

        if (simpleRenderable->showOutline) {
            terrain_translate(&rect, transform->pos.x, transform->pos.y, simpleRenderable->outlineWidth, simpleRenderable->outlineHeight);
            Texture_Draw(simpleRenderable->spriteOutline, (int)rect.x, (int)rect.y, rect.w, rect.h, transform->angle);
        }

        terrain_translate(&rect, transform->pos.x, transform->pos.y, simpleRenderable->width, simpleRenderable->height);
        Texture_ColorMod(simpleRenderable->sprite, ((Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID))->color);
        Texture_Draw(simpleRenderable->sprite, (int)rect.x, (int)rect.y, rect.w, rect.h, transform->angle);
    }
}