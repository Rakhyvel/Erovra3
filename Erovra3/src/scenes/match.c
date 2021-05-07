#pragma once
#include "match.h"
#include "../components/bullet.h"
#include "../components/city.h"
#include "../components/coin.h"
#include "../components/components.h"
#include "../components/infantry.h"
#include "../components/mine.h"
#include "../components/nation.h"
#include "../components/ore.h"
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../gui/gui.h"
#include "../main.h"
#include "../terrain.h"
#include "../textures.h"
#include <stdio.h>
#include <string.h>

Terrain* terrain;
EntityID container;
EntityID goldLabel;
EntityID metalLabel;
EntityID populationLabel;

void Match_DetectHit(struct scene* scene)
{
    const ComponentMask renderMask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, UNIT_COMPONENT_ID, HEALTH_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, renderMask); Scene_End(scene, id); id = Scene_Next(scene, id, renderMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Health* health = (Unit*)Scene_GetComponent(scene, id, HEALTH_COMPONENT_ID);

        if (simpleRenderable->hitTicks > 0) {
            simpleRenderable->hitTicks--;
        }
        health->aliveTicks++;

        ComponentID otherNation = GET_COMPONENT_FIELD(scene, simpleRenderable->nation, NATION_COMPONENT_ID, Nation, enemyNationFlag);

        // Find closest enemy projectile
        const ComponentMask otherMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, otherNation, PROJECTILE_COMPONENT_ID);
        EntityID otherID;
        for (otherID = Scene_Begin(scene, otherMask); Scene_End(scene, otherID); otherID = Scene_Next(scene, otherID, otherMask)) {
            Motion* otherMotion = (Motion*)Scene_GetComponent(scene, otherID, MOTION_COMPONENT_ID);
            Projectile* projectile = (Projectile*)Scene_GetComponent(scene, otherID, PROJECTILE_COMPONENT_ID);
            float dist = Vector_Dist(motion->pos, otherMotion->pos);
            if (dist < 8) {
                health->health -= projectile->attack / unit->defense;
                simpleRenderable->hitTicks = 18;
                Scene_MarkPurged(scene, otherID);
                if (health->health <= 0) {
                    Scene_MarkPurged(scene, id);
                }
            }
        }
    }
}

/*
	Takes in a scene, iterates through all entites that have a motion component. 
	Their position is then incremented by their velocity. */
void Match_Motion(struct scene* scene)
{
    const ComponentMask motionMask = Scene_CreateMask(1, MOTION_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, motionMask); Scene_End(scene, id); id = Scene_Next(scene, id, motionMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        motion->pos = Vector_Add(motion->pos, motion->vel);
        float height = terrain_getHeight(terrain, (int)motion->pos.x, (int)motion->pos.y);
        if (height == -1) {
            Scene_MarkPurged(scene, id);
        }
    }
}

/*
	Takes in a scene, iterates through all entities that have a target component.
	First checks that the angle is correct, and then sets the velocity according
	to the target vector accordingly */
void Match_Target(struct scene* scene)
{
    const ComponentMask motionMask = Scene_CreateMask(2, MOTION_COMPONENT_ID, TARGET_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, motionMask); Scene_End(scene, id); id = Scene_Next(scene, id, motionMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

        // Calculate if pointing in direction of lookat vector
        Vector displacement = Vector_Sub(motion->pos, target->lookat);
        float tempAngle = Vector_Angle(displacement); // atan2
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
        } else if (Vector_Dist(target->tar, motion->pos) > motion->speed) {
            // Looking in direction, not at target, move
            motion->vel.x = (target->tar.x - motion->pos.x);
            motion->vel.y = (target->tar.y - motion->pos.y);
            float mag = sqrtf((motion->vel.x * motion->vel.x) + (motion->vel.y * motion->vel.y));
            if (mag > 0.1) {
                motion->vel.x /= mag / motion->speed;
                motion->vel.y /= mag / motion->speed;
            }

            displacement = Vector_Add((motion->pos), (motion->vel));
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
	Calculates whether a unit is hovered over with the mouse. Assumes rectangle 
	shape, given by SimpleRenderable texture bounds */
void Match_Hover(struct scene* scene)
{
    EntityID id;
    EntityID hoveredID = INVALID_ENTITY_INDEX;
    const ComponentMask hoverMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, HOVERABLE_COMPONENT_ID);
    for (id = Scene_Begin(scene, hoverMask); Scene_End(scene, id); id = Scene_Next(scene, id, hoverMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Hoverable* hoverable = (Hoverable*)Scene_GetComponent(scene, id, HOVERABLE_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);

        Vector mouse = Terrain_MousePos();
        float dx = motion->pos.x - mouse.x;
        float dy = mouse.y - motion->pos.y + motion->z;

        float sin = sinf(motion->angle);
        float cos = cosf(motion->angle);

        bool checkLR = fabs(sin * dx + cos * dy) <= simpleRenderable->height / 2;
        bool checkTB = fabs(cos * dx - sin * dy) <= simpleRenderable->width / 2;

        hoverable->isHovered = checkLR && checkTB;
        if (hoverable->isHovered) {
            hoveredID = id;
        }
        simpleRenderable->showOutline = false;
    }
    if (hoveredID != INVALID_ENTITY_INDEX) {
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, hoveredID, SIMPLE_RENDERABLE_COMPONENT_ID);
        simpleRenderable->showOutline = true;
    }
}

/*
	Checks to see if the escape key is pressed. If it is, all selectables are 
	deselected */
void Match_ClearSelection(struct scene* scene)
{
    if (g->keys[SDL_SCANCODE_ESCAPE]) {
        const ComponentMask transformMask = Scene_CreateMask(1, SELECTABLE_COMPONENT_ID);
        EntityID id;
        for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
            Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
            selectable->selected = false;
        }
    }
}

/*
	Checks to see if the escape key is pressed, and if it is, clears all focused 
	units */
void Match_ClearFocus(struct scene* scene)
{
    if (g->keys[SDL_SCANCODE_ESCAPE]) {
        const ComponentMask mask = Scene_CreateMask(1, FOCUSABLE_COMPONENT_ID);
        EntityID id;
        for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
            Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
            focusable->focused = false;
            GUI_SetContainerShown(scene, focusable->guiContainer, false);
        }
    }
}

/*
	 Iterates through entities that are selectable. Determines if a unit is 
	 hovered, selected, if a whole task force is selected, and if and where to 
	 set units' targets. */
void Match_Select(struct scene* scene)
{
    bool targeted = false;
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
                    centerOfMass = Vector_Add(centerOfMass, motion->pos);
                }
                numSelected++;
            }
            if (numSelected != 0) {
                centerOfMass = Vector_Scalar(centerOfMass, 1.0f / numSelected);
            }
        }
        for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
            Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
            Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
            Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
            if (selectable->selected) {
                Vector mouse = Terrain_MousePos();
                if (g->shift) { // Offset by center of mass, calculated earlier
                    Vector distToCenter = Vector_Sub(motion->pos, centerOfMass);
                    mouse = Vector_Add(mouse, distToCenter);
                }
                target->tar = mouse;
                target->lookat = mouse;
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
        const ComponentMask transformMask = Scene_CreateMask(3, SELECTABLE_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, HOVERABLE_COMPONENT_ID);
        EntityID id;
        bool anySelected = false;
        for (id = Scene_Begin(scene, transformMask); Scene_End(scene, id); id = Scene_Next(scene, id, transformMask)) {
            Selectable* selectable = (Selectable*)Scene_GetComponent(scene, id, SELECTABLE_COMPONENT_ID);
            Hoverable* hoverable = (Hoverable*)Scene_GetComponent(scene, id, HOVERABLE_COMPONENT_ID);
            SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
            simpleRenderable->showOutline = hoverable->isHovered || selectable->selected;
            if (hoverable->isHovered && g->mouseLeftUp) {
                selectable->selected = !selectable->selected;
                anySelected |= selectable->selected;
                break;
            }
        }
    }
}

/*
	When the right mouse button is released, finds the focusable entity that 
	is hovered, and shows its GUI */
void Match_Focus(struct scene* scene)
{
    if (g->mouseRightUp) {
        EntityID focusedEntity = INVALID_ENTITY_INDEX;
        const ComponentMask focusMask = Scene_CreateMask(3, FOCUSABLE_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, HOVERABLE_COMPONENT_ID);
        EntityID id;
        for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
            Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
            Hoverable* hoverable = (Hoverable*)Scene_GetComponent(scene, id, HOVERABLE_COMPONENT_ID);
            SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
            if (hoverable->isHovered) {
                focusedEntity = id;
            }
        }
        // The focused entity's gui container should be shown, all others should not be. If the focused entity is none, all should be hidden
        EntityID containerID = INVALID_ENTITY_INDEX;
        for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
            Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
            if (containerID != INVALID_ENTITY_INDEX && focusable->guiContainer == containerID) {
                continue;
            }
            if (focusedEntity == id) {
                containerID = focusable->guiContainer;
            }
            focusable->focused = focusedEntity == id;
            GUI_SetContainerShown(scene, focusable->guiContainer, focusable->focused);
        }
    }
}

/*
	This system goes through all ground units, has them search for the closest 
	enemy land unit to them, and finally shoot a bullet at them. */
void Match_Attack(struct scene* scene)
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
            float dist = Vector_Dist(otherMotion->pos, motion->pos);
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
        Vector displacement = Vector_Sub(motion->pos, closestPos);
        float deflection = Vector_Angle(displacement);
        if (g->ticks % 60 == 0 && fabs(deflection - motion->angle) < 0.2 * motion->speed) {
            Bullet_Create(scene, motion->pos, closestPos, unit->attack, simpleRenderable->nation);
        }
    }
}

void Match_CreateCoins(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, CITY_COMPONENT_ID, HEALTH_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        City* city = (Motion*)Scene_GetComponent(scene, id, CITY_COMPONENT_ID);
        Health* health = (Motion*)Scene_GetComponent(scene, id, HEALTH_COMPONENT_ID);

        if (health->aliveTicks % 300 == 0) {
            Coin_Create(scene, motion->pos, simpleRenderable->nation);
        }
        if (health->aliveTicks % 18000 == 0) {
            Infantry_Create(scene, motion->pos, simpleRenderable->nation);
        }
    }
}

void Match_DestroyCoins(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, COIN_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Motion* capital = (Motion*)Scene_GetComponent(scene, nation->capital, MOTION_COMPONENT_ID);

        if (Vector_Dist(motion->pos, capital->pos) < 6) {
            Scene_MarkPurged(scene, id);
            nation->coins++;
        }
    }
}

void Match_CreateOre(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, MINE_COMPONENT_ID, HEALTH_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Health* health = (Motion*)Scene_GetComponent(scene, id, HEALTH_COMPONENT_ID);

        int ticks = (int)(300.0 / terrain_getOre(terrain, motion->pos.x, motion->pos.y));
        if (health->aliveTicks % ticks == 0) {
            Ore_Create(scene, motion->pos, simpleRenderable->nation);
        }
    }
}

void Match_DestroyOre(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, ORE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Motion* capital = (Motion*)Scene_GetComponent(scene, nation->capital, MOTION_COMPONENT_ID);

        if (Vector_Dist(motion->pos, capital->pos) < 6) {
            Scene_MarkPurged(scene, id);
            nation->ore++;
        }
    }
}

/*
	Takes in a scene, iterates through all entities with SimpleRenderable and 
	Transform components. Translates texture based on Terrain's offset and zoom,
	colorizes based on the nation color, and renders texture to screen. */
void Match_SimpleRender(struct scene* scene)
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
            Texture_AlphaMod(simpleRenderable->spriteOutline, (Uint8)255);
            terrain_translate(&rect, motion->pos.x, motion->pos.y - 2, simpleRenderable->outlineWidth, simpleRenderable->outlineHeight);
            Texture_Draw(simpleRenderable->spriteOutline, (int)rect.x, (int)rect.y, rect.w, rect.h, motion->angle);
        } else if (simpleRenderable->hitTicks > 0) {
            Texture_AlphaMod(simpleRenderable->spriteOutline, (Uint8)(simpleRenderable->hitTicks / 18.0f * 255));
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

void Match_UpdateLabels(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, NATION_COMPONENT_ID, HOME_NATION_FLAG_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Nation* nation = (Nation*)Scene_GetComponent(scene, id, NATION_COMPONENT_ID);

        char goldText[255];
        memset(goldText, 0, 255);
        char goldAmount[255];
        _itoa_s(nation->coins, goldAmount, 255, 10);
        strcat_s(goldText, 255, goldAmount);
        GUI_SetLabelText(scene, goldLabel, goldText);

		char metalText[255];
        memset(metalText, 0, 255);
        char metalAmount[255];
        _itoa_s((int)(nation->ore), metalAmount, 255, 10);
        strcat_s(metalText, 255, metalAmount);
        GUI_SetLabelText(scene, metalLabel, metalText);

        char populationText[255];
        memset(populationText, 0, 255);
        char populationAmount[255];
        _itoa_s((int)(nation->population), populationAmount, 255, 10);
        strcat_s(populationText, 255, populationAmount);
        GUI_SetLabelText(scene, populationLabel, populationText);
    }
}

void matchUpdate(Scene* match)
{
    terrain_update(terrain);
    Match_DetectHit(match);

    Match_Target(match);
    Match_Motion(match);

    Match_ClearSelection(match);
    Match_ClearFocus(match);
    Match_Hover(match);
    Match_Select(match);
    Match_Focus(match);

    Match_Attack(match);

    Match_CreateCoins(match);
    Match_DestroyCoins(match);
    Match_CreateOre(match);
    Match_DestroyOre(match);

    GUI_Update(match);
}

void matchRender(Scene* match)
{
    terrain_render(terrain);
    Match_UpdateLabels(match);
    Match_SimpleRender(match);
    GUI_Render(match);
}

/*
	Called when infantry build city button is pressed. Builds a city */
void Match_InfantryAddCity(Scene* scene)
{
    const ComponentMask focusMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, FOCUSABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        if (focusable->focused && terrain_getHeightForBuilding(terrain, motion->pos.x, motion->pos.y) > 0.5 && terrain_closestMaskDist(scene, Scene_CreateMask(1, CITY_COMPONENT_ID), terrain, motion->pos.x, motion->pos.y) > 2 && nation->coins >= nation->cityCost) {
            EntityID city = City_Create(scene, (Vector) { 64 * (int)(motion->pos.x / 64) + 32, 64 * (int)(motion->pos.y / 64) + 32 }, simpleRenderable->nation, false);
            terrain_addBuildingAt(terrain, city, motion->pos.x, motion->pos.y);
            nation->coins -= nation->cityCost;
            nation->cityCost *= 2;
            nation->population++;
        }
    }
}

void Match_InfantryAddMine(Scene* scene)
{
    const ComponentMask focusMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, FOCUSABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        if (focusable->focused && terrain_getHeightForBuilding(terrain, motion->pos.x, motion->pos.y) > 0.5 && terrain_closestBuildingDist(terrain, motion->pos.x, motion->pos.y) > 0 && nation->coins >= nation->mineCost) {
            EntityID mine = Mine_Create(scene, (Vector) { 64 * (int)(motion->pos.x / 64) + 32, 64 * (int)(motion->pos.y / 64) + 32 }, simpleRenderable->nation, false);
            terrain_addBuildingAt(terrain, mine, motion->pos.x, motion->pos.y);
            nation->coins -= nation->mineCost;
            nation->mineCost *= 2;
        }
    }
}

void Match_InfantryTestSoil(Scene* scene)
{
    const ComponentMask focusMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, FOCUSABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        if (focusable->focused) {
            printf("Ore: %f\n", terrain->ore[(int)(motion->pos.x / 64) + (int)(motion->pos.y / 64) * terrain->tileSize]);
        }
    }
}

/*
	Creates a new scene, adds in two nations, capitals for those nations, and infantries for those nation */
Scene* Match_Init()
{
    Scene* match = Scene_Create(Components_Init, &matchUpdate, &matchRender);
    terrain = terrain_create(12 * 64);
    GUI_Init(match);

    container = GUI_CreateContainer(match, (Vector) { 100, 100 });
    goldLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    metalLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    populationLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    GUI_ContainerAdd(match, container, goldLabel);
    GUI_ContainerAdd(match, container, metalLabel);
    GUI_ContainerAdd(match, container, populationLabel);

    INFANTRY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 10, 100 });
    GUI_ContainerAdd(match, container, INFANTRY_FOCUSED_GUI);
    GUI_ContainerAdd(match, INFANTRY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Build City", &Match_InfantryAddCity));
    GUI_ContainerAdd(match, INFANTRY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Build Mine", &Match_InfantryAddMine));
    GUI_ContainerAdd(match, INFANTRY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Test Soil", &Match_InfantryTestSoil));
    GUI_SetContainerShown(match, INFANTRY_FOCUSED_GUI, false);

    CITY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 10, 100 });
    GUI_ContainerAdd(match, container, CITY_FOCUSED_GUI);
    GUI_ContainerAdd(match, CITY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Pause Recruitment", NULL));
    GUI_SetContainerShown(match, CITY_FOCUSED_GUI, false);

    // Create home and enemy nations
    EntityID homeNation = Nation_Create(match, (SDL_Color) { 60, 100, 250 }, HOME_NATION_FLAG_COMPONENT_ID, ENEMY_NATION_FLAG_COMPONENT_ID);
    EntityID enemyNation = Nation_Create(match, (SDL_Color) { 250, 80, 80 }, ENEMY_NATION_FLAG_COMPONENT_ID, HOME_NATION_FLAG_COMPONENT_ID);

    // Create and register home city
    Vector homeVector = findBestLocation(terrain, (Vector) { terrain->size, terrain->size });
    EntityID homeCapital = City_Create(match, homeVector, homeNation, true);
    terrain_addBuildingAt(terrain, homeCapital, homeVector.x, homeVector.y);
    terrain_setOffset(homeVector);

    // Create and register enemy city
    Vector enemyVector = findBestLocation(terrain, (Vector) { 0, 0 });
    EntityID enemyCapital = City_Create(match, enemyVector, enemyNation, true);
    terrain_addBuildingAt(terrain, enemyCapital, enemyVector.x, enemyVector.y);

    // Create home and enemy infantry
    EntityID homeInfantry = Infantry_Create(match, GET_COMPONENT_FIELD(match, homeCapital, MOTION_COMPONENT_ID, Motion, pos), homeNation);
    EntityID enemyInfantry = Infantry_Create(match, GET_COMPONENT_FIELD(match, enemyCapital, MOTION_COMPONENT_ID, Motion, pos), enemyNation);

    // Set enemy nations to each other
    SET_COMPONENT_FIELD(match, homeNation, NATION_COMPONENT_ID, Nation, enemyNation, enemyNation);
    SET_COMPONENT_FIELD(match, enemyNation, NATION_COMPONENT_ID, Nation, enemyNation, homeNation);

    // Set nations capitals
    SET_COMPONENT_FIELD(match, homeNation, NATION_COMPONENT_ID, Nation, capital, homeCapital);
    SET_COMPONENT_FIELD(match, enemyNation, NATION_COMPONENT_ID, Nation, capital, enemyCapital);
    Game_PushScene(match);

    return match;
}