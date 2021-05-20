#pragma once
#include "match.h"
#include "../components/artillery.h"
#include "../components/bullet.h"
#include "../components/cavalry.h"
#include "../components/city.h"
#include "../components/coin.h"
#include "../components/components.h"
#include "../components/factory.h"
#include "../components/infantry.h"
#include "../components/mine.h"
#include "../components/nation.h"
#include "../components/ore.h"
#include "../components/shell.h"
#include "../components/wall.h"
#include "../engine/gameState.h"
#include "../engine/textureManager.h"
#include "../gui/gui.h"
#include "../main.h"
#include "../terrain.h"
#include "../textures.h"
#include "../util/debug.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

Terrain* terrain;
EntityID container;

// Nation labels
EntityID goldLabel;
EntityID oreLabel;
EntityID populationLabel;

// Factory gui elements
EntityID orderLabel;
EntityID timeLabel;
EntityID autoReOrderRockerSwitch;

static const int cityPop = 3;
static const float taxRate = 0.25f;
static const int ticksPerLabor = 400;

// UTILITY FUNCTIONS

/*
	Takes in a producer, and a unit. Depending on the resources of the producer's
	nation, either sets the producer's order, or does nothing. */
bool Match_PlaceOrder(Scene* scene, Nation* nation, Producer* producer, UnitType type)
{
    for (int i = 0; i < _ResourceType_Length; i++) {
        if (nation->resources[i] < nation->costs[i][type]) {
            return false;
        }
    }
    for (int i = 0; i < _ResourceType_Length; i++) {
        nation->resources[i] -= nation->costs[i][type];
    }
    producer->orderTicksRemaining = nation->costs[ResourceType_COIN][type] * ticksPerLabor;
    producer->order = type;
    return true;
}

bool Match_BuyCity(struct scene* scene, EntityID nationID, Vector pos)
{
    Nation* nation = (Nation*)Scene_GetComponent(scene, nationID, NATION_COMPONENT_ID);
    pos.x = (int)(pos.x / 64) * 64 + 32;
    pos.y = (int)(pos.y / 64) * 64 + 32;
    if (terrain_getHeightForBuilding(terrain, pos.x, pos.y) > 0.5 && terrain_closestMaskDist(scene, Scene_CreateMask(1, CITY_COMPONENT_ID), terrain, pos.x, pos.y) > 2 && terrain_closestBuildingDist(terrain, pos.x, pos.y) > 0 && nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_CITY]) {
        EntityID city = City_Create(scene, (Vector) { pos.x, pos.y }, nationID, false);
        terrain_addBuildingAt(terrain, city, pos.x, pos.y);
        nation->resources[ResourceType_COIN] -= nation->costs[ResourceType_COIN][UnitType_CITY];
        nation->costs[ResourceType_COIN][UnitType_CITY] *= 2;
        nation->resources[ResourceType_POPULATION_CAPACITY] += cityPop;
        return true;
    }
    return false;
}

bool Match_BuyMine(struct scene* scene, EntityID nationID, Vector pos)
{
    Nation* nation = (Nation*)Scene_GetComponent(scene, nationID, NATION_COMPONENT_ID);
    pos.x = (int)(pos.x / 64) * 64 + 32;
    pos.y = (int)(pos.y / 64) * 64 + 32;
    if (terrain_getHeightForBuilding(terrain, pos.x, pos.y) > 0.5 && terrain_getBuildingAt(terrain, pos.x, pos.y) == INVALID_ENTITY_INDEX && nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_MINE]) {
        EntityID mine = Mine_Create(scene, (Vector) { pos.x, pos.y }, nationID, false);
        terrain_addBuildingAt(terrain, mine, pos.x, pos.y);
        nation->resources[ResourceType_COIN] -= nation->costs[ResourceType_COIN][UnitType_MINE];
        nation->costs[ResourceType_COIN][UnitType_MINE] *= 2;
        nation->resources[ResourceType_POPULATION] += 1;
        return true;
    }
    return false;
}

bool Match_BuyFactory(struct scene* scene, EntityID nationID, Vector pos)
{
    Nation* nation = (Nation*)Scene_GetComponent(scene, nationID, NATION_COMPONENT_ID);
    pos.x = (int)(pos.x / 64) * 64 + 32;
    pos.y = (int)(pos.y / 64) * 64 + 32;

    EntityID homeCity = terrain_adjacentMask(scene, Scene_CreateMask(1, CITY_COMPONENT_ID), terrain, pos.x, pos.y);
    if (terrain_getHeightForBuilding(terrain, pos.x, pos.y) > 0.5 && homeCity != INVALID_ENTITY_INDEX && terrain_closestBuildingDist(terrain, pos.x, pos.y) > 0 && nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FACTORY]) {
        EntityID factory = Factory_Create(scene, (Vector) { pos.x, pos.y }, nationID, homeCity);
        terrain_addBuildingAt(terrain, factory, pos.x, pos.y);
        nation->resources[ResourceType_COIN] -= nation->costs[ResourceType_COIN][UnitType_FACTORY];
        nation->costs[ResourceType_COIN][UnitType_FACTORY] *= 2;
        nation->resources[ResourceType_POPULATION] += 1;
        return true;
    }
    return false;
}

// SYSTEMS

/*
	Checks each health entity against all projectile entites. If the two are 
	collided, the health of the entity is reduced based on the projectile's 
	attack, and the health's defense. 
	
	Some projectiles have splash damage. If so, the damage of the projectile 
	falls off as the distance from the projectile increases 
	
	If the entity is a wall or building, and is destroyed, will remove from 
	either the wall or building map in the terrain struct */
void Match_DetectHit(struct scene* scene)
{
    const ComponentMask renderMask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, UNIT_COMPONENT_ID, HEALTH_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, renderMask); Scene_End(scene, id); id = Scene_Next(scene, id, renderMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Health* health = (Unit*)Scene_GetComponent(scene, id, HEALTH_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);

        if (simpleRenderable->hitTicks > 0) {
            simpleRenderable->hitTicks--;
        }
        health->aliveTicks++;

        ComponentID otherNationID = GET_COMPONENT_FIELD(scene, simpleRenderable->nation, NATION_COMPONENT_ID, Nation, enemyNationFlag);

        // Find closest enemy projectile
        const ComponentMask otherMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, otherNationID, PROJECTILE_COMPONENT_ID);
        EntityID otherID;
        Nation* otherNation = NULL;
        SimpleRenderable* otherSimpleRenderable = NULL;
        bool deadFlag = false;
        for (otherID = Scene_Begin(scene, otherMask); Scene_End(scene, otherID); otherID = Scene_Next(scene, otherID, otherMask)) {
            Motion* otherMotion = (Motion*)Scene_GetComponent(scene, otherID, MOTION_COMPONENT_ID);
            Projectile* projectile = (Projectile*)Scene_GetComponent(scene, otherID, PROJECTILE_COMPONENT_ID);
            otherSimpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, otherID, SIMPLE_RENDERABLE_COMPONENT_ID);
            otherNation = (Nation*)Scene_GetComponent(scene, otherSimpleRenderable->nation, NATION_COMPONENT_ID);

            float dist = Vector_Dist(motion->pos, otherMotion->pos);
            if (projectile->armed && dist < projectile->splash) {
                float splashDamageModifier;
                if (projectile->splash <= 8) {
                    splashDamageModifier = 1.0f; // Damage is same regardless of distance
                } else {
                    splashDamageModifier = 1.0f - dist / projectile->splash; // The farther away from splash damage, the less damage it does
                }
                health->health -= projectile->attack * splashDamageModifier / unit->defense;
                // Building set engaged ticks
                if (Scene_EntityHasComponent(scene, Scene_CreateMask(1, BUILDING_FLAG_COMPONENT_ID), id) || Scene_EntityHasComponent(scene, Scene_CreateMask(1, WALL_FLAG_COMPONENT_ID), id)) {
                    nation->visitedSpaces[(int)(motion->pos.x / 32) + (int)(motion->pos.y / 32) * nation->visitedSpacesSize] = -1;
                    unit->engagedTicks = 100000;
                }
                simpleRenderable->hitTicks = 18;
                Scene_MarkPurged(scene, otherID);

                if (health->health <= 0) {
                    deadFlag = true;
                    break;
                }
            }
        }

        if (deadFlag) {
            if (!Scene_EntityHasComponent(scene, Scene_CreateMask(1, CITY_COMPONENT_ID), id)) {
                Scene_MarkPurged(scene, id);
                // Remove from wall map if wall
                if (Scene_EntityHasComponent(scene, Scene_CreateMask(1, WALL_FLAG_COMPONENT_ID), id)) {
                    terrain_addWallAt(terrain, INVALID_ENTITY_INDEX, motion->pos.x, motion->pos.y);
                }
                // Remove from building map if building
                if (Scene_EntityHasComponent(scene, Scene_CreateMask(1, BUILDING_FLAG_COMPONENT_ID), id)) {
                    terrain_addBuildingAt(terrain, INVALID_ENTITY_INDEX, motion->pos.x, motion->pos.y);
                }
                if (Scene_EntityHasComponent(scene, Scene_CreateMask(1, GROUND_UNIT_FLAG_COMPONENT_ID), id)) {
                    nation->resources[ResourceType_POPULATION]--;
                }
            } else if (otherNation != NULL && otherSimpleRenderable != NULL) {
                nation->resources[ResourceType_POPULATION_CAPACITY] -= cityPop;
                otherNation->resources[ResourceType_POPULATION_CAPACITY] += cityPop;
                simpleRenderable->nation = otherSimpleRenderable->nation;
                Scene_Unassign(scene, id, HOME_NATION_FLAG_COMPONENT_ID);
                Scene_Unassign(scene, id, ENEMY_NATION_FLAG_COMPONENT_ID);
                Scene_Assign(scene, id, otherNation->ownNationFlag, NULL);
                health->health = 100;
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

void Match_SetVisitedSpace(struct scene* scene)
{
    const ComponentMask motionMask = Scene_CreateMask(2, MOTION_COMPONENT_ID, UNIT_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, motionMask); Scene_End(scene, id); id = Scene_Next(scene, id, motionMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);

        if (!unit->stuckIn && !unit->engaged) {
            nation->visitedSpaces[(int)(motion->pos.x / 32) + (int)(motion->pos.y / 32) * nation->visitedSpacesSize] = 11000;
        }
    }
}

/*
	Takes in a scene, iterates through all entities that have a target component.
	First checks that the angle is correct, and then sets the velocity according
	to the target vector accordingly
	
	Stops units if they go through an enemy wall */
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
            // Hit water -> stay still
            if (height < motion->z || height > motion->z + 0.5f) {
                motion->vel.x = 0;
                motion->vel.y = 0;
            }

            // Check for enemy walls
            SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
            Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
            const ComponentMask wallMask = Scene_CreateMask(2, WALL_FLAG_COMPONENT_ID, nation->enemyNationFlag);
            EntityID wallID;
            for (wallID = Scene_Begin(scene, wallMask); Scene_End(scene, wallID); wallID = Scene_Next(scene, wallID, wallMask)) {
                Motion* wallMotion = (Motion*)Scene_GetComponent(scene, wallID, MOTION_COMPONENT_ID);
                float beforeDiff = 0;
                float afterDiff = 0;
                if (wallMotion->angle != 0 && motion->pos.y < wallMotion->pos.y + 32 && motion->pos.y > wallMotion->pos.y - 32) {
                    beforeDiff = motion->pos.x - wallMotion->pos.x;
                    afterDiff = motion->pos.x - wallMotion->pos.x + motion->vel.x;
                } else if (motion->pos.x < wallMotion->pos.x + 32 && motion->pos.x > wallMotion->pos.x - 32) {
                    beforeDiff = motion->pos.y - wallMotion->pos.y;
                    afterDiff = motion->pos.y - wallMotion->pos.y + motion->vel.y;
                }
                if (beforeDiff < 0 && afterDiff > 0 || beforeDiff > 0 && afterDiff < 0) {
                    motion->vel.x = 0;
                    motion->vel.y = 0;
                }
            }
        } else {
            motion->vel.x = 0;
            motion->vel.y = 0;
        }

        while (motion->angle > M_PI * 2) {
            motion->angle -= M_PI * 2;
        }
        while (motion->angle < 0) {
            motion->angle += M_PI * 2;
        }
    }
}

/*
	This system checks to see if a shell has reached it's desitantion. If it has, 
	if the shell isn't armed, arms the shell. If the shell is already armed, it 
	destroys the shell entity. */
void Match_ShellMove(struct scene* scene)
{
    const ComponentMask motionMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, PROJECTILE_COMPONENT_ID, SHELL_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, motionMask); Scene_End(scene, id); id = Scene_Next(scene, id, motionMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Projectile* projectile = (Projectile*)Scene_GetComponent(scene, id, PROJECTILE_COMPONENT_ID);
        Shell* shell = (Shell*)Scene_GetComponent(scene, id, SHELL_COMPONENT_ID);

        float dist = Vector_Dist(motion->pos, shell->tar);
        if (dist < 8) {
            if (!projectile->armed) {
                projectile->armed = true;
            } else {
                Scene_MarkPurged(scene, id);
            }
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
    const ComponentMask hoverMask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, HOVERABLE_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID);
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
        const ComponentMask transformMask = Scene_CreateMask(2, SELECTABLE_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID);
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
        const ComponentMask mask = Scene_CreateMask(2, PLAYER_FLAG_COMPONENT_ID, FOCUSABLE_COMPONENT_ID);
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
        const ComponentMask transformMask = Scene_CreateMask(4, SELECTABLE_COMPONENT_ID, TARGET_COMPONENT_ID, MOTION_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID);
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
	is hovered, and shows its GUI. */
void Match_Focus(struct scene* scene)
{
    if (g->mouseRightUp) {
        EntityID focusedEntity = INVALID_ENTITY_INDEX;
        const ComponentMask focusMask = Scene_CreateMask(4, FOCUSABLE_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, HOVERABLE_COMPONENT_ID, PLAYER_FLAG_COMPONENT_ID);
        EntityID id;
        // Go through all focusable entities, find the last one that is hovered, set focusedEntity to it
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
                focusable->focused = false;
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
	Updates the visited spaces for a nation.

	All spaces will decay to 0, representing the certainty that there are no enemy
	units falling.

	Any space that is negative represents a space that has a known enemy unit.
	These are reset every tick. */
void Match_AIUpdateVisitedSpaces(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, NATION_COMPONENT_ID, AI_FLAG_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Nation* nation = (Nation*)Scene_GetComponent(scene, id, NATION_COMPONENT_ID);

        for (int y = 0; y < nation->visitedSpacesSize; y++) {
            for (int x = 0; x < nation->visitedSpacesSize; x++) {
                if (nation->visitedSpaces[x + y * nation->visitedSpacesSize] > 0) {
                    nation->visitedSpaces[x + y * nation->visitedSpacesSize] -= 0.1;
                } else if (nation->visitedSpaces[x + y * nation->visitedSpacesSize] > -1) {
                    nation->visitedSpaces[x + y * nation->visitedSpacesSize] = 0;
                }
            }
        }
    }
}

/*
	 */
void Match_AIGroundUnitTarget(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(5, MOTION_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, GROUND_UNIT_FLAG_COMPONENT_ID, AI_FLAG_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);

        float dist = Vector_Dist(motion->pos, target->tar);
        if (unit->engaged) {
            continue;
        }

        Vector closestTile = { -1, -1 };
        float tempDist = FLT_MAX;
        bool foundEnemy = false;
        for (int y = 0; y < nation->visitedSpacesSize; y++) {
            for (int x = 0; x < nation->visitedSpacesSize; x++) {
                Vector point = { x * 32 + 16, y * 32 + 16 };
                // Tile must be unvisited
                if (nation->visitedSpaces[x + y * nation->visitedSpacesSize] > 0)
                    continue;
                // If there is an enemy, don't go to unknown spaces, go to enemy spaces
                if (foundEnemy && nation->visitedSpaces[x + y * nation->visitedSpacesSize] > -1)
                    continue;

                float score = Vector_Dist(motion->pos, point);

                // Must be land
                if (terrain_getHeight(terrain, point.x, point.y) <= 0.5)
                    continue;

                // Must have direct line of sight to tile center
                if ((score < tempDist || (!foundEnemy && nation->visitedSpaces[x + y * nation->visitedSpacesSize] <= -1)) && terrain_lineOfSight(terrain, motion->pos, point)) {
                    tempDist = score;
                    closestTile = point;
                    // If enemy is found
                    if (nation->visitedSpaces[x + y * nation->visitedSpacesSize] <= -1) {
                        foundEnemy = true;
                    }
                }
            }
        }

        if (closestTile.x != -1) {
            float newDist = Vector_Dist(target->tar, closestTile);
            if (newDist != dist && foundEnemy) {
                target->tar = closestTile;
                target->lookat = closestTile;
                continue;
            }
        }
        if (dist > 1) {
            continue;
        }

        // An empty space could not be found, set unit's target randomly
        float randX = (float)(rand()) / (float)RAND_MAX - 0.5;
        float randY = (float)(rand()) / (float)RAND_MAX - 0.5;
        Vector newTarget = Vector_Add(motion->pos, Vector_Scalar(Vector_Normalize((Vector) { randX, randY }), 64));
        if (terrain_lineOfSight(terrain, motion->pos, newTarget)) {
            target->tar = newTarget;
            target->lookat = newTarget;
        }
    }
}

void Match_AIInfantryBuild(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, INFANTRY_UNIT_FLAG_COMPONENT_ID, AI_FLAG_COMPONENT_ID);
    EntityID id;
    ComponentMask nationsDone = 0;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        if ((nationsDone & simpleRenderable->nation) == simpleRenderable->nation) {
            continue;
        }
        if (unit->engaged) {
            continue;
        }
        if (unit->stuckIn) {
            continue;
        }

        // Find closest city point, build city if on point
        if (nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_CITY]) {
            float tempDistance = FLT_MAX;
            Vector tempTarget = { -1, -1 };
            for (int y = 0; y < terrain->tileSize; y++) {
                for (int x = 0; x < terrain->tileSize; x++) {
                    if (terrain_getBuildingAt(terrain, x * 64 + 32, y * 64 + 32) != INVALID_ENTITY_INDEX)
                        continue;
                    Vector point = { x * 64 + 32, y * 64 + 32 };
                    // Find if there is a city with cabdist less than 3 tiles
                    const ComponentMask cityMask = Scene_CreateMask(1, CITY_COMPONENT_ID);
                    EntityID cityID;
                    bool exitFlag = false;
                    for (cityID = Scene_Begin(scene, cityMask); Scene_End(scene, cityID); cityID = Scene_Next(scene, cityID, cityMask)) {
                        Motion* cityMotion = (Motion*)Scene_GetComponent(scene, cityID, MOTION_COMPONENT_ID);
                        if (Vector_CabDist(point, cityMotion->pos) < 3 * 64) {
                            exitFlag = true;
                            break;
                        }
                    }
                    if (exitFlag) {
                        continue;
                    }
                    if (terrain_getHeight(terrain, point.x, point.y) < 0.5)
                        continue;
                    double distance = Vector_Dist(motion->pos, point) - terrain_getHeight(terrain, point.x, point.y) * 10;
                    if (distance > tempDistance)
                        continue;
                    if (!terrain_lineOfSight(terrain, motion->pos, point))
                        continue;
                    tempTarget = point;
                    tempDistance = distance;
                }
            }
            // City point found, set target
            if (tempTarget.x != -1) {
                target->tar = tempTarget;
                target->lookat = tempTarget;
                if (Vector_Dist(motion->pos, target->tar) < 32) {
                    Match_BuyCity(scene, simpleRenderable->nation, motion->pos);
                }
                nationsDone |= simpleRenderable->nation;
                continue;
            }
        }
        // Find closest mine point, build mine if on mine point
        if (nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_MINE]) {
            float tempDistance = FLT_MAX;
            Vector tempTarget = (Vector) { -1, -1 };
            for (int y = 0; y < terrain->tileSize; y++) {
                for (int x = 0; x < terrain->tileSize; x++) {
                    if (terrain_getBuildingAt(terrain, x * 64, y * 64) != INVALID_ENTITY_INDEX)
                        continue;

                    // Only go to empty squares
                    Vector point = { x * 64 + 32, y * 64 + 32 };
                    if (terrain_getHeight(terrain, point.x, point.y) < 0.5)
                        continue;

                    float distance = Vector_Dist(motion->pos, point);
                    if (distance > tempDistance)
                        continue;

                    if (!terrain_lineOfSight(terrain, motion->pos, point))
                        continue;

                    tempTarget = point;
                    tempDistance = distance;
                }
            }
            // Factory point found, build mine
            if (tempTarget.x != -1) {
                target->tar = tempTarget;
                target->lookat = tempTarget;
                if (Vector_Dist(motion->pos, target->tar) < 32) {
                    if (Match_BuyMine(scene, simpleRenderable->nation, motion->pos)) {
                        nationsDone |= simpleRenderable->nation;
                        continue;
                    }
                }
            }
        }
        // Find closest factory point, build factory if on factory point
        if (nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FACTORY]) {
            float tempDistance = FLT_MAX;
            Vector tempTarget = (Vector) { -1, -1 };
            for (int y = 0; y < terrain->tileSize; y++) {
                for (int x = 0; x < terrain->tileSize; x++) {
                    if (terrain_getBuildingAt(terrain, x * 64, y * 64) != INVALID_ENTITY_INDEX)
                        continue;

                    // Only go to empty squares
                    Vector point = { x * 64 + 32, y * 64 + 32 };
                    const ComponentMask cityMask = Scene_CreateMask(1, CITY_COMPONENT_ID);
                    EntityID cityID;
                    bool foundFlag = false;
                    for (cityID = Scene_Begin(scene, cityMask); Scene_End(scene, cityID); cityID = Scene_Next(scene, cityID, cityMask)) {
                        Motion* cityMotion = (Motion*)Scene_GetComponent(scene, cityID, MOTION_COMPONENT_ID);
                        if (Vector_CabDist(point, cityMotion->pos) == 64) {
                            foundFlag = true;
                            break;
                        }
                    }
                    if (!foundFlag)
                        continue;

                    if (terrain_getHeight(terrain, point.x, point.y) < 0.5)
                        continue;

                    float distance = Vector_Dist(motion->pos, point);
                    if (distance > tempDistance)
                        continue;

                    if (!terrain_lineOfSight(terrain, motion->pos, point))
                        continue;

                    tempTarget = point;
                    tempDistance = distance;
                }
            }
            // Factory point found, build factory
            if (tempTarget.x != -1) {
                target->tar = tempTarget;
                target->lookat = tempTarget;
                if (Vector_Dist(motion->pos, target->tar) < 32) {
                    Match_BuyFactory(scene, simpleRenderable->nation, motion->pos);
                }
                nationsDone |= simpleRenderable->nation;
                continue;
            }
        }
    }
}

void Match_AIOrderUnits(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, PRODUCER_COMPONENT_ID, AI_FLAG_COMPONENT_ID);
    EntityID id;
    ComponentMask nationsDone = 0;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        if (producer->orderTicksRemaining > 0) {
            continue;
        }
        if (rand() % 2 == 0) {
            Match_PlaceOrder(scene, nation, producer, UnitType_ARTILLERY);
        } else {
            Match_PlaceOrder(scene, nation, producer, UnitType_CAVALRY);
        }
    }
}

/*
	This system goes through all combatants, has them search for the closest 
	enemy to them, and finally shoot a projectile at them. */
void Match_CombatantAttack(struct scene* scene)
{
    const ComponentMask renderMask = Scene_CreateMask(5, MOTION_COMPONENT_ID, TARGET_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, renderMask); Scene_End(scene, id); id = Scene_Next(scene, id, renderMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Health* health = (Health*)Scene_GetComponent(scene, id, HEALTH_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Combatant* combatant = (Combatant*)Scene_GetComponent(scene, id, COMBATANT_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = (Unit*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);

        // Find closest enemy ground unit
        float closestDist = combatant->attackDist;
        EntityID closest = INVALID_ENTITY_INDEX;
        Vector closestPos = { -1, -1 };
        const ComponentMask otherMask = Scene_CreateMask(1, MOTION_COMPONENT_ID) | combatant->enemyMask;
        EntityID otherID;
        bool groundUnit = false;
        for (otherID = Scene_Begin(scene, otherMask); Scene_End(scene, otherID); otherID = Scene_Next(scene, otherID, otherMask)) {
            Motion* otherMotion = (Motion*)Scene_GetComponent(scene, otherID, MOTION_COMPONENT_ID);
            float dist = Vector_Dist(otherMotion->pos, motion->pos);
            if (dist < closestDist) {
                closestDist = dist;
                closest = otherID;
                closestPos = otherMotion->pos;
                groundUnit = Scene_EntityHasComponent(scene, Scene_CreateMask(1, GROUND_UNIT_FLAG_COMPONENT_ID), otherID);
                int x = (int)closestPos.x / 32;
                int y = (int)closestPos.y / 32;
                nation->visitedSpaces[x + (y * nation->visitedSpacesSize)] = -1;
                if (x > 0)
                    nation->visitedSpaces[x - 1 + (y * nation->visitedSpacesSize)] = -1;
                if (y > 0)
                    nation->visitedSpaces[x + (y - 1) * nation->visitedSpacesSize] = -1;
                if (x < (terrain->tileSize * 2) - 1)
                    nation->visitedSpaces[x + 1 + y * nation->visitedSpacesSize] = -1;
                if (y < (terrain->tileSize * 2) - 1)
                    nation->visitedSpaces[x + (y + 1) * nation->visitedSpacesSize] = -1;
            }
        }

        // If no enemy units were found, stuckin and engaged are false, skip
        if (closest == INVALID_ENTITY_INDEX) {
            if (unit->stuckIn) {
                target->lookat = target->tar;
            }
            unit->stuckIn = false;
            unit->engaged = false;
            continue;
        }

        // Set flags indicating that unit is engaged in battle
        if (groundUnit) {
            target->tar = motion->pos;
        }
        target->lookat = closestPos;
        unit->stuckIn = true;
        unit->engaged = true;
        unit->engagedTicks = (int)(128.0f / motion->speed);

        // Shoot enemy units if found
        Vector displacement = Vector_Sub(motion->pos, closestPos);
        float deflection = Vector_Angle(displacement);

        while (deflection > M_PI * 2) {
            deflection -= M_PI * 2;
        }
        while (deflection < 0) {
            deflection += M_PI * 2;
        }
        if (health->aliveTicks % combatant->attackTime == 0 && fabs(deflection - motion->angle) < 0.2 * motion->speed) {
            combatant->projConstructor(scene, motion->pos, closestPos, combatant->attack, simpleRenderable->nation);
        }
    }
}

/*
	Every time a unit of labor has passed, one coin is added.
	
	An infantry is spawned every 5 minutes. */
void Match_UpdateCity(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, CITY_COMPONENT_ID, HEALTH_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        City* city = (Motion*)Scene_GetComponent(scene, id, CITY_COMPONENT_ID);
        Health* health = (Motion*)Scene_GetComponent(scene, id, HEALTH_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);

        if (nation->resources[ResourceType_POPULATION] < nation->resources[ResourceType_POPULATION_CAPACITY] && health->aliveTicks % 18000 == 0) {
            Infantry_Create(scene, motion->pos, simpleRenderable->nation);
            nation->resources[ResourceType_POPULATION]++;
        }
    }
}

/*
	For every resource particle, if the particle is at the capital, marks particle for purge, increases
	nation's resources by 1. 
	
	Could have a flag for resource particle, loop through those entities in one system. Nations
	would have a resource array, particles would contain an index for which resource they were. Would
	increase that resource for the nations resource array */
void Match_DestroyResourceParticles(struct scene* scene)
{
    ComponentMask mask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, RESOURCE_PARTICLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        ResourceParticle* resourceParticle = (ResourceParticle*)Scene_GetComponent(scene, id, RESOURCE_PARTICLE_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Motion* capital = (Motion*)Scene_GetComponent(scene, nation->capital, MOTION_COMPONENT_ID);

        if (Vector_Dist(motion->pos, capital->pos) < 6) {
            Scene_MarkPurged(scene, id);
            nation->resources[resourceParticle->type]++;
        }
    }
}

/*
	Every unit of labor ticks, mines create one ore for capital */
void Match_ProduceResources(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(1, RESOURCE_PRODUCER_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Health* health = (Motion*)Scene_GetComponent(scene, id, HEALTH_COMPONENT_ID);
        ResourceProducer* resourceProducer = (ResourceProducer*)Scene_GetComponent(scene, id, RESOURCE_PRODUCER_COMPONENT_ID);

        int ticks = (int)(ticksPerLabor * resourceProducer->produceRate);
        if (health->aliveTicks % ticks == 0) {
            resourceProducer->particleConstructor(scene, motion->pos, simpleRenderable->nation);
        }
    }
}

/*
	For every producer, decrements time left for producer. If there are no ticks 
	left, produces the unit. 
	
	If the producer's "repeat" flag is set, repeats the order a second time. */
void Match_Produce(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, PRODUCER_COMPONENT_ID, FOCUSABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);

        if (nation->resources[ResourceType_POPULATION] < nation->resources[ResourceType_POPULATION_CAPACITY] && --producer->orderTicksRemaining == 0) {
            if (producer->order == UnitType_INFANTRY) {
                Infantry_Create(scene, motion->pos, simpleRenderable->nation);
            } else if (producer->order == UnitType_CAVALRY) {
                Cavalry_Create(scene, motion->pos, simpleRenderable->nation);
            } else if (producer->order == UnitType_ARTILLERY) {
                Artillery_Create(scene, motion->pos, simpleRenderable->nation);
            } else {
                PANIC("Producer's can't build that UnitType");
            }
            nation->resources[ResourceType_POPULATION]++;

            if (!producer->repeat) {
                producer->order = INVALID_ENTITY_INDEX;
                GUI_SetContainerShown(scene, focusable->guiContainer, false);
                focusable->guiContainer = FACTORY_READY_FOCUSED_GUI;
                GUI_SetContainerShown(scene, focusable->guiContainer, focusable->focused);
            }
        } else if (producer->repeat && producer->orderTicksRemaining < 0) {
            Match_PlaceOrder(scene, nation, producer, producer->order);
        }
    }
}

void Match_UpdateProducerAllegiance(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(1, PRODUCER_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        if (!Scene_EntityHasComponent(scene, Scene_CreateMask(1, CITY_COMPONENT_ID), id)) {
            SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
            Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
            SimpleRenderable* homeCitySimpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, producer->homeCity, SIMPLE_RENDERABLE_COMPONENT_ID);
            simpleRenderable->nation = homeCitySimpleRenderable->nation;
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
        if (simpleRenderable->hidden) {
            continue;
        }

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

/*
	Called every tick. Sets the text of labels to reflect the game state 
	
	Updates produce GUI to reflect the time, order, and auto-order */
void Match_UpdateGUIElements(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, NATION_COMPONENT_ID, HOME_NATION_FLAG_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Nation* nation = (Nation*)Scene_GetComponent(scene, id, NATION_COMPONENT_ID);

        GUI_SetLabelText(scene, goldLabel, "Coins: %d", nation->resources[ResourceType_COIN]);
        GUI_SetLabelText(scene, oreLabel, "Ore: %d", nation->resources[ResourceType_ORE]);
        GUI_SetLabelText(scene, populationLabel, "Population: %d/%d", nation->resources[ResourceType_POPULATION], nation->resources[ResourceType_POPULATION_CAPACITY]);

        const ComponentMask focusMask = Scene_CreateMask(1, FOCUSABLE_COMPONENT_ID);
        EntityID focusID;
        for (focusID = Scene_Begin(scene, focusMask); Scene_End(scene, focusID); focusID = Scene_Next(scene, focusID, focusMask)) {
            Focusable* focusable = (Focusable*)Scene_GetComponent(scene, focusID, FOCUSABLE_COMPONENT_ID);

            if (focusable->focused && Scene_EntityHasComponent(scene, Scene_CreateMask(1, PRODUCER_COMPONENT_ID), focusID)) {
                Producer* producer = (Producer*)Scene_GetComponent(scene, focusID, PRODUCER_COMPONENT_ID);
                GUI_SetLabelText(scene, orderLabel, "Order: %d", producer->order);
                GUI_SetLabelText(scene, timeLabel, "Time remaining: %d", producer->orderTicksRemaining);
                GUI_SetRockerSwitchValue(scene, autoReOrderRockerSwitch, producer->repeat);
            }
        }
    }
}

/*
	Goes through each unit AI entity unit. Updates engaged ticks. 
	Units are hidden when their engaged ticks are less than 0. */
void Match_UpdateFogOfWar(struct scene* scene)
{
    const ComponentMask mask = Scene_CreateMask(2, UNIT_COMPONENT_ID, AI_FLAG_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, mask); Scene_End(scene, id); id = Scene_Next(scene, id, mask)) {
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);

        unit->engagedTicks--;
        //simpleRenderable->hidden = unit->engagedTicks < 0;
    }
}

void matchUpdate(Scene* match)
{
    terrain_update(terrain);

    Match_AIUpdateVisitedSpaces(match);

    Match_DetectHit(match);

    Match_ClearSelection(match);
    Match_ClearFocus(match);
    Match_Hover(match);
    Match_Select(match);
    Match_Focus(match);

    Match_AIGroundUnitTarget(match);
    Match_AIOrderUnits(match);
    Match_AIInfantryBuild(match);

    Match_Target(match);
    Match_Motion(match);
    Match_ShellMove(match);
    Match_SetVisitedSpace(match);
    Match_CombatantAttack(match);

    Match_ProduceResources(match);
    Match_DestroyResourceParticles(match);
    Match_Produce(match);
    Match_UpdateProducerAllegiance(match);

    GUI_Update(match);

    // Change game tick speed
    if (g->lt) {
        g->dt *= 2.0;
        printf("%f\n", g->dt);
    } else if (g->gt) {
        g->dt *= 0.5;
        printf("%f\n", g->dt);
    } else if (g->ctrl) {
        Game_PopScene(match);
    }
}

void matchRender(Scene* match)
{
    terrain_render(terrain);
    Match_UpdateFogOfWar(match);
    Match_SimpleRender(match);
    Match_UpdateGUIElements(match);
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
        if (focusable->focused) {
            Match_BuyCity(scene, simpleRenderable->nation, motion->pos);
        }
    }
}

/*
	Called by the infantry's "Build Factory" button. Builds a mine */
void Match_InfantryAddMine(Scene* scene)
{
    const ComponentMask focusMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, FOCUSABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        if (focusable->focused) {
            Match_BuyMine(scene, simpleRenderable->nation, motion->pos);
        }
    }
}

/*
	Called by the infantry's "Build Factory" button. Builds a factory */
void Match_InfantryAddFactory(Scene* scene)
{
    const ComponentMask focusMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, FOCUSABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        if (focusable->focused) {
            Match_BuyFactory(scene, simpleRenderable->nation, motion->pos);
        }
    }
}

/*
	Called by the infantry's "Build Wall" button. Builds a wall on the gridline 
	segment that the infantry is closest to. Doesn't add a wall if there is 
	already a wall in place. */
void Match_InfantryAddWall(Scene* scene)
{
    const ComponentMask focusMask = Scene_CreateMask(3, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, FOCUSABLE_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        if (focusable->focused && nation->resources[ResourceType_COIN] >= 15) {
            Vector cellMidPoint = { 64 * (int)(motion->pos.x / 64) + 32, 64 * (int)(motion->pos.y / 64) + 32 };
            float angle;
            float xOffset = cellMidPoint.x - motion->pos.x;
            float yOffset = cellMidPoint.y - motion->pos.y;
            // Central wall, with units orientation
            if (xOffset * xOffset + yOffset * yOffset < 15 * 15) {
                if ((motion->angle < M_PI / 4 && motion->angle > 0) || motion->angle > 7 * M_PI / 4 || (motion->angle > 3 * M_PI / 4 && motion->angle < 5 * M_PI / 4)) {
                    angle = 0;
                } else {
                    angle = M_PI / 2;
                }
                if (terrain_getBuildingAt(terrain, cellMidPoint.x, cellMidPoint.y) != INVALID_ENTITY_INDEX) {
                    continue;
                }
            }
            // Upward orientation
            else if (abs(xOffset) > abs(yOffset)) {
                if (xOffset > 0) {
                    cellMidPoint.x -= 32;
                } else {
                    cellMidPoint.x += 32;
                }
                angle = 3.1415926 / 2;
            }
            // Sideways orientation
            else {
                if (yOffset > 0) {
                    cellMidPoint.y -= 32;
                } else {
                    cellMidPoint.y += 32;
                }
                angle = 0;
            }

            if (terrain_getWallAt(terrain, cellMidPoint.x, cellMidPoint.y) == INVALID_ENTITY_INDEX) {
                EntityID wall = Wall_Create(scene, cellMidPoint, angle, simpleRenderable->nation, false);
                terrain_addWallAt(terrain, wall, cellMidPoint.x, cellMidPoint.y);
                nation->resources[ResourceType_COIN] -= 15;
            }
        }
    }
}

/*
	Called by the infantry's "Test Soil" button. Gives the user the info for the soil */
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
	Called by factory "Build Cavalry" button. Sets the producer that is focused to produce cavalry */
void Match_FactoryOrderCavalry(Scene* scene)
{
    const ComponentMask focusMask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, FOCUSABLE_COMPONENT_ID, PRODUCER_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);

        if (focusable->focused && Match_PlaceOrder(scene, nation, producer, UnitType_CAVALRY)) {
            GUI_SetContainerShown(scene, focusable->guiContainer, false);
            focusable->guiContainer = FACTORY_BUSY_FOCUSED_GUI;
            GUI_SetContainerShown(scene, focusable->guiContainer, true);
        }
    }
}

/*
	Called by factory "Build Artillery" button. Sets the producer that is focused to producer artillery */
void Match_FactoryOrderArtillery(Scene* scene)
{
    const ComponentMask focusMask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, FOCUSABLE_COMPONENT_ID, PRODUCER_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);

        if (focusable->focused && Match_PlaceOrder(scene, nation, producer, UnitType_ARTILLERY)) {
            GUI_SetContainerShown(scene, focusable->guiContainer, false);
            focusable->guiContainer = FACTORY_BUSY_FOCUSED_GUI;
            GUI_SetContainerShown(scene, focusable->guiContainer, true);
        }
    }
}

/*
	Called from factory "Cancel Order" button. Cancels the order of the Producer that is focused */
void Match_FactoryCancelOrder(Scene* scene)
{
    const ComponentMask focusMask = Scene_CreateMask(4, MOTION_COMPONENT_ID, SIMPLE_RENDERABLE_COMPONENT_ID, FOCUSABLE_COMPONENT_ID, PRODUCER_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);

        if (focusable->focused) {
            producer->orderTicksRemaining = -1;
            GUI_SetContainerShown(scene, focusable->guiContainer, false);
            focusable->guiContainer = FACTORY_READY_FOCUSED_GUI;
            GUI_SetContainerShown(scene, focusable->guiContainer, true);
        }
    }
}

/*
	RockerSwitch callback that updates the repeat value of a producer based on the
	value of the rocker switch */
void Match_FactoryReOrder(Scene* scene, bool value)
{
    const ComponentMask focusMask = Scene_CreateMask(2, FOCUSABLE_COMPONENT_ID, PRODUCER_COMPONENT_ID);
    EntityID id;
    for (id = Scene_Begin(scene, focusMask); Scene_End(scene, id); id = Scene_Next(scene, id, focusMask)) {
        Focusable* focusable = (Focusable*)Scene_GetComponent(scene, id, FOCUSABLE_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);

        if (focusable->focused) {
            producer->repeat = value;
        }
    }
}

/*
	Creates a new scene, adds in two nations, capitals for those nations, and infantries for those nation */
Scene* Match_Init()
{
    Scene* match = Scene_Create(Components_Init, &matchUpdate, &matchRender);
    terrain = terrain_create(12 * 64); // 8 is smallest for good reasons
    GUI_Init(match);

    container = GUI_CreateContainer(match, (Vector) { 100, 100 });
    goldLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    oreLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    populationLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    orderLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    timeLabel = GUI_CreateLabel(match, (Vector) { 0, 0 }, "");
    GUI_ContainerAdd(match, container, goldLabel);
    GUI_ContainerAdd(match, container, oreLabel);
    GUI_ContainerAdd(match, container, populationLabel);

    INFANTRY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 10, 100 });
    GUI_ContainerAdd(match, container, INFANTRY_FOCUSED_GUI);
    GUI_ContainerAdd(match, INFANTRY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Build City", &Match_InfantryAddCity));
    GUI_ContainerAdd(match, INFANTRY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Build Mine", &Match_InfantryAddMine));
    GUI_ContainerAdd(match, INFANTRY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Build Factory", &Match_InfantryAddFactory));
    GUI_ContainerAdd(match, INFANTRY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Build Wall", &Match_InfantryAddWall));
    GUI_ContainerAdd(match, INFANTRY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Test Soil", &Match_InfantryTestSoil));
    GUI_SetContainerShown(match, INFANTRY_FOCUSED_GUI, false);

    FACTORY_READY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 10, 100 });
    GUI_ContainerAdd(match, container, FACTORY_READY_FOCUSED_GUI);
    GUI_ContainerAdd(match, FACTORY_READY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Build Cavalry", &Match_FactoryOrderCavalry));
    GUI_ContainerAdd(match, FACTORY_READY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Build Artillery", &Match_FactoryOrderArtillery));
    GUI_SetContainerShown(match, FACTORY_READY_FOCUSED_GUI, false);

    FACTORY_BUSY_FOCUSED_GUI = GUI_CreateContainer(match, (Vector) { 10, 100 });
    GUI_ContainerAdd(match, container, FACTORY_BUSY_FOCUSED_GUI);
    GUI_ContainerAdd(match, FACTORY_BUSY_FOCUSED_GUI, GUI_CreateLabel(match, (Vector) { 0, 0 }, "Factory is busy. Come back later."));
    GUI_ContainerAdd(match, FACTORY_BUSY_FOCUSED_GUI, orderLabel);
    GUI_ContainerAdd(match, FACTORY_BUSY_FOCUSED_GUI, timeLabel);
    GUI_ContainerAdd(match, FACTORY_BUSY_FOCUSED_GUI, GUI_CreateButton(match, (Vector) { 100, 100 }, 150, 50, "Cancel Order", &Match_FactoryCancelOrder));
    GUI_ContainerAdd(match, FACTORY_BUSY_FOCUSED_GUI, autoReOrderRockerSwitch = GUI_CreateRockerSwitch(match, (Vector) { 100, 100 }, "Auto Re-order", false, &Match_FactoryReOrder));
    GUI_SetContainerShown(match, FACTORY_BUSY_FOCUSED_GUI, false);

    // Create home and enemy nations
    EntityID homeNation = Nation_Create(match, (SDL_Color) { 60, 100, 250 }, terrain->size, HOME_NATION_FLAG_COMPONENT_ID, ENEMY_NATION_FLAG_COMPONENT_ID, AI_FLAG_COMPONENT_ID);
    EntityID enemyNation = Nation_Create(match, (SDL_Color) { 250, 80, 80 }, terrain->size, ENEMY_NATION_FLAG_COMPONENT_ID, HOME_NATION_FLAG_COMPONENT_ID, AI_FLAG_COMPONENT_ID);

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
    Nation_SetCapital(match, homeNation, homeCapital);
    Nation_SetCapital(match, enemyNation, enemyCapital);

    Game_PushScene(match);

    return match;
}