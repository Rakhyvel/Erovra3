#pragma once
#include "systems.h"
#include "../components/bullet.h"
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../main.h"
#include "../terrain.h"
#include "../textures.h"
#include <stdio.h>

void System_Motion(struct terrain* terrain, struct scene* scene)
{
    const ComponentMask motionMask = Scene_CreateMask(1, MOTION_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, motionMask); Scene_End(scene, id); id = Scene_Next(scene, id, motionMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        motion->pos = Vector_Add(&motion->pos, &motion->vel);
        float height = terrain_getHeight(terrain, (int)motion->pos.x, (int)motion->pos.y);
        if (height == -1) {
            Scene_MarkPurged(scene, id);
        }
    }
}

/*
	Takes in a scene, iterates through all entities that have a transform
	component. First checks the rotation, if rotation is off, will correct 
	angle before moving. Otherwise, updates the velocity based on target 
	and position, and postion based on target. */
void System_Target(struct terrain* terrain, struct scene* scene)
{
    const ComponentMask motionMask = Scene_CreateMask(2, MOTION_COMPONENT_ID, TARGET_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, motionMask); Scene_End(scene, id); id = Scene_Next(scene, id, motionMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

        // Calculate if pointing in direction of lookat vector
        Vector displacement = Vector_Sub(&motion->pos, &target->lookat);
        float tempAngle = Vector_Angle(&displacement); // atan2
        if (fabs(motion->angle - tempAngle) < 3.1415926) {
            // Can be compared directly
        } else if (motion->angle < tempAngle) {
            motion->angle += 2 * 3.1415926;
        } else if (motion->angle > tempAngle) {
            motion->angle -= 2 * 3.1415926;
        }
        float diff = fabs(motion->angle - tempAngle);

        if (diff > motion->speed / 18.0f) {
            // Not looking in direction, turn
            if (motion->angle > tempAngle) {
                motion->angle -= motion->speed / 18.0f;
            } else {
                motion->angle += motion->speed / 18.0f;
            }
            motion->vel.x = 0;
            motion->vel.y = 0;
        } else if (Vector_Dist(&target->tar, &motion->pos) > motion->speed) {
            // Looking in direction, not at target, move
            motion->vel.x = (target->tar.x - motion->pos.x);
            motion->vel.y = (target->tar.y - motion->pos.y);
            float mag = sqrtf((motion->vel.x * motion->vel.x) + (motion->vel.y * motion->vel.y));
            if (mag > 0.1) {
                motion->vel.x /= mag / motion->speed;
                motion->vel.y /= mag / motion->speed;
            }

            displacement = Vector_Add(&(motion->pos), &(motion->vel));
            float height = terrain_getHeight(terrain, (int)displacement.x, (int)displacement.y);
            if (height < motion->z || height > motion->z + 0.5f) {
                motion->vel.x = 0;
                motion->vel.y = 0;
            }
        } else {
            motion->vel.x = 0;
            motion->vel.y = 0;
        }
    }
}

/*
	 Iterates through entities that are selectable. Determines if a unit is 
	 hovered, selected, if a whole task force is selected, and if and where to 
	 set units' targets. */
void System_Select(struct scene* scene)
{
    bool targeted = false;

    // If escape is pressed, go through all entities, make them not selected
    if (g->keys[SDL_SCANCODE_ESCAPE]) {
        const ComponentMask transformMask = Scene_CreateMask(1, SELECTABLE_COMPONENT_ID);
        EntityID id;
        for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
            Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
            selectable->selected = false;
        }
    }

    // If ctrl is not clicked, go through entities, if they are selected, set their target
    if (!g->ctrl && g->mouseLeftUp && !g->mouseDragged) {
        const ComponentMask transformMask = Scene_CreateMask(3, SELECTABLE_COMPONENT_ID, TARGET_COMPONENT_ID, MOTION_COMPONENT_ID);
        EntityID id;
        Vector centerOfMass = { 0, 0 };
        // If shift is held down, find center of mass of selected units
        if (g->shift) {
            int numSelected = 0;
            for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
                Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
                Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
                if (selectable->selected) {
                    centerOfMass = Vector_Add(&centerOfMass, &motion->pos);
                }
                numSelected++;
            }
            if (numSelected != 0) {
                centerOfMass = Vector_Scalar(&centerOfMass, 1.0f / numSelected);
            }
        }
        for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
            Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
            Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
            Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
            if (selectable->selected) {
                Vector mouse = Terrain_MousePos();
                if (g->shift) { // Offset by center of mass, calculated earlier
                    Vector distToCenter = Vector_Sub(&motion->pos, &centerOfMass);
                    mouse = Vector_Add(&mouse, &distToCenter);
                }
                Vector_Copy(&target->tar, &mouse);
                Vector_Copy(&target->lookat, &mouse);
                if (!g->keys[SDL_SCANCODE_S]) { // Check if should (s)tandby for more orders
                    selectable->selected = false;
                }
                targeted = true;
            }
        }
    }

    Selectable* hovered = NULL;
    // If no unit targets were set previously, go thru entities, check to see if they are now hovered and selected
    if (!targeted) {
        const ComponentMask transformMask = Scene_CreateMask(3, SELECTABLE_COMPONENT_ID, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID);
        EntityID id;
        for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
            Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
            Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
            SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);

            Vector mouse = Terrain_MousePos();
            float dx = motion->pos.x - mouse.x;
            float dy = mouse.y - motion->pos.y + motion->z;

            float sin = sinf(motion->angle);
            float cos = cosf(motion->angle);

            bool checkLR = fabs(sin * dx + cos * dy) <= simpleRenderable->height / 2;
            bool checkTB = fabs(cos * dx - sin * dy) <= simpleRenderable->width / 2;

            selectable->isHovered = checkLR && checkTB;

            simpleRenderable->showOutline = selectable->isHovered || selectable->selected;
            if (g->mouseLeftUp && selectable->isHovered) {
                selectable->selected = !selectable->selected;
                break;
            }
        }
    }
}

void System_Attack(struct scene* scene)
{
    const ComponentMask renderMask = Scene_CreateMask(5, MOTION_COMPONENT_ID, TARGET_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, UNIT_COMPONENT_ID, GROUND_UNIT_FLAG_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, renderMask); Scene_End(scene, id); id = Scene_Next(scene, id, renderMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        ComponentID otherNation = GET_COMPONENT_FIELD(scene, simpleRenderable->nation, NATION_COMPONENT_ID, Nation, enemyNationFlag);

        // Find closest enemy ground unit
        float closestDist = 68;
        EntityID closest = INVALID_ENTITY_INDEX;
        Vector closestPos = { -1, -1 };
        const ComponentMask otherMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, otherNation, LAND_UNIT_FLAG_COMPONENT_ID);
        EntityID otherID;
        for (otherID = Scene_Begin(scene, otherMask); Scene_End(scene, otherID); otherID = Scene_Next(scene, otherID, otherMask)) {
            Motion* otherMotion = (Motion*)Scene_GetComponent(scene, otherID, MOTION_COMPONENT_ID);
            float dist = Vector_Dist(&otherMotion->pos, &motion->pos);
            if (dist < closestDist) {
                closestDist = dist;
                closest = otherID;
                closestPos = otherMotion->pos;
            }
        }

        // If no enemy units were found, stuckin and engaged are false, skip
        if (closest == INVALID_ENTITY_INDEX) {
            unit->stuckIn = false;
            unit->engaged = false;
            continue;
        }
        // If enemies were within 48, tar = pos, stuck in
        if (closestDist < 48) {
            target->tar = motion->pos;
            target->lookat = closestPos;
            unit->stuckIn = true;
        }
        // engaged = true, engagedTicks = whatever
        unit->engaged = true;

        // Shoot enemy units if found
		Vector displacement = Vector_Sub(&motion->pos, &closestPos);
        float deflection = Vector_Angle(&displacement);
        if (ticks % 60 == 0 && fabs(deflection - motion->angle) < 0.2 * motion->speed) {
            Bullet_Create(scene, motion->pos, closestPos, simpleRenderable->nation);
        }
    }
}

/*
	Takes in a scene, iterates through all entities with SimpleRenderable and 
	Transform components. Translates texture based on Terrain's offset and zoom,
	colorizes based on the nation color, and renders texture to screen. */
void System_Render(struct scene* scene)
{
    const ComponentMask renderMask = Scene_CreateMask(2, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, renderMask); Scene_End(scene, id); id = Scene_Next(scene, id, renderMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        SDL_FRect rect = { 0, 0, 0, 0 };

        terrain_translate(&rect, motion->pos.x, motion->pos.y, simpleRenderable->width, simpleRenderable->height);
        Texture_Draw(simpleRenderable->shadow, (int)rect.x, (int)rect.y, rect.w, rect.h, motion->angle);

        if (simpleRenderable->showOutline) {
            terrain_translate(&rect, motion->pos.x, motion->pos.y - 2, simpleRenderable->outlineWidth, simpleRenderable->outlineHeight);
            Texture_Draw(simpleRenderable->spriteOutline, (int)rect.x, (int)rect.y, rect.w, rect.h, motion->angle);
        }

        terrain_translate(&rect, motion->pos.x, motion->pos.y - 2, simpleRenderable->width, simpleRenderable->height);
        if (!motion->destroyOnBounds) {
            Texture_ColorMod(simpleRenderable->sprite, ((Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID))->color);
        }
        Texture_Draw(simpleRenderable->sprite, (int)rect.x, (int)rect.y, rect.w, rect.h, motion->angle);
    }
}