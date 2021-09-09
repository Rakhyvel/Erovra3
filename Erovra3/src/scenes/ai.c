#pragma once
#include "ai.h";
#include "../components/components.h"
#include "../engine/gameState.h"
#include "./match.h"
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

void AI_UpdateVariables(Scene* scene, Goap* goap, ComponentKey key)
{
    system(scene, id, NATION_COMPONENT_ID, key)
    {
        Nation* nation = (Nation*)Scene_GetComponent(scene, id, NATION_COMPONENT_ID);

        // How many ticks does it take to make an ore
        const float averageTicksPerOreMade = nation->unitCount[UnitType_MINE] == 0 ? 54000.0f : ticksPerLabor / nation->unitCount[UnitType_MINE];
        // How many ticks does it take to use an ore
        const float averageTicksPerOreUsed = nation->unitCount[UnitType_FACTORY] == 0 ? 54000.0f : (ticksPerLabor * 20.0f) / (1.0f * nation->unitCount[UnitType_FACTORY]);
        // Build a mine if it takes more ticks to make an ore than it does to use one (Different from coins below)

        // How many ticks does it take to make a coin
        const float averageTicksPerCoinMade = nation->unitCount[UnitType_CITY] == 0 ? 54000.0f : ticksPerLabor / nation->unitCount[UnitType_CITY];
        // How many ticks does it take to use a coin
        const int coinUsers = nation->unitCount[UnitType_FACTORY] + nation->unitCount[UnitType_ACADEMY] + nation->unitCount[UnitType_PORT];
        const float averageTicksPerCoinUsed = coinUsers == 0 ? 54000.0f : (ticksPerLabor * 15.0f) / (15.0f * coinUsers);
        // Build a factory if it takes less ticks to make a coin than it does to use one (Different from ore above)
        // makeCoinTicks < useCoinTicks

        goap->variables[NO_KNOWN_ENEMY_UNITS] = true;
        goap->variables[FOUND_ENEMY_CAPITAL] = false;
        goap->variables[COMBATANTS_AT_ENEMY_CAPITAL] = false;

        goap->variables[HAS_CAVALRY] = nation->unitCount[UnitType_CAVALRY] > max(1, 0 /*known enemy cavalry*/);
        goap->variables[HAS_INFANTRY] = nation->unitCount[UnitType_INFANTRY] > max(1, 0 /*known enemy infantry*/);
        goap->variables[HAS_COINS] = averageTicksPerCoinMade < averageTicksPerCoinUsed;
        goap->variables[HAS_ORE] = averageTicksPerOreMade < averageTicksPerOreUsed;
        goap->variables[HAS_POPULATION] = nation->resources[ResourceType_POPULATION] < nation->resources[ResourceType_POPULATION_CAPACITY];

        goap->variables[AFFORD_INFANTRY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_INFANTRY];
        goap->variables[AFFORD_CAVALRY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_CAVALRY];
        goap->variables[AFFORD_CAVALRY_ORE] = nation->resources[ResourceType_ORE] >= nation->costs[ResourceType_ORE][UnitType_CAVALRY];

        goap->variables[AFFORD_CITY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_CITY];
        goap->variables[AFFORD_MINE_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_MINE];
        goap->variables[AFFORD_FACTORY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FACTORY];
        goap->variables[AFFORD_FARM_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FARM];
        goap->variables[AFFORD_ACADEMY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_ACADEMY];
    }

    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID, key)
    {
        Motion* motion = (Target*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

        goap->variables[ENGINEER_ISNT_BUSY] = Vector_Dist(motion->pos, target->tar) < 32;
    }

    // Update
    goap->variables[HAS_AVAILABLE_FACTORY] = false;
    goap->variables[HAS_AVAILABLE_ACADEMY] = false;
    system(scene, id, UNIT_COMPONENT_ID, PRODUCER_COMPONENT_ID, key)
    {
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);

        if (producer->orderTicksRemaining < 0) {
            switch (unit->type) {
            case UnitType_FACTORY:
                goap->variables[HAS_AVAILABLE_FACTORY] = true;
                break;
            case UnitType_ACADEMY:
                goap->variables[HAS_AVAILABLE_ACADEMY] = true;
                break;
            }
        }
    }
}

void AI_TargetGroundUnitsRandomly(Scene* scene, ComponentKey key)
{
    printf("Target ground units randomly\n");

    system(scene, id, MOTION_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID, key)
    {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);

        if (unit->engaged) {
            continue;
        }
        bool isPatrol = Scene_EntityHasComponents(scene, id, PATROL_COMPONENT_ID);
        Patrol* patrol = NULL;
        if (isPatrol) {
            patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
        }

        float dist = isPatrol ? Vector_Dist(motion->pos, patrol->patrolPoint) : Vector_Dist(motion->pos, target->tar);

        // Search for an alerted tile
        Vector closestTile = { -1, -1 };
        float tempDist = FLT_MAX;
        bool foundEnemy = false;
        bool cityDefense = false;
        for (int y = 0; y < nation->visitedSpacesSize; y++) {
            for (int x = 0; x < nation->visitedSpacesSize; x++) {
                // Skip all other tiles if a city needs to be defended
                if (cityDefense) {
                    continue;
                }
                Vector point = { x * 32.0f, y * 32.0f };
                // Tile must be unvisited
                if (nation->visitedSpaces[x + y * nation->visitedSpacesSize] > 0)
                    continue;
                bool enemySpace = nation->visitedSpaces[x + y * nation->visitedSpacesSize] <= -1;
                // City emergency defense (drop everything and help)
                if (nation->visitedSpaces[x + y * nation->visitedSpacesSize] <= -10 && Terrain_LineOfSight(terrain, motion->pos, point, motion->z)) {
                    closestTile = point;
                    cityDefense = true;
                }
                // If there is an enemy, don't go to unknown spaces, go to enemy spaces
                if (foundEnemy && !enemySpace)
                    continue;

                Motion* capital = (Motion*)Scene_GetComponent(scene, nation->capital, MOTION_COMPONENT_ID);

                float score = Vector_Dist(motion->pos, point) + 64 * 5 * Vector_Dist(motion->pos, capital->pos) + (float)rand() / (float)RAND_MAX;

                // Must have direct line of sight to tile center
                if ((score < tempDist || (!foundEnemy && enemySpace)) && Terrain_LineOfSight(terrain, motion->pos, point, motion->z)) {
                    tempDist = score;
                    closestTile = point;
                    // If enemy is found
                    if (enemySpace) {
                        foundEnemy = true;
                    }
                }
            }
        }
        if (closestTile.x != -1) {
            float newDist = Vector_Dist(motion->pos, closestTile);
            if (dist < 3 || (newDist < dist && foundEnemy)) {
                if (isPatrol) {
                    patrol->patrolPoint = closestTile;
                } else {
                    target->tar = closestTile;
                    target->lookat = closestTile;
                }
            }
        } else if (dist < 1) { // An alerted space could not be found, set unit's target randomly
            float randX = (float)(rand()) / (float)RAND_MAX - 0.5f;
            float randY = (float)(rand()) / (float)RAND_MAX - 0.5f;
            Vector newTarget = Vector_Add(motion->pos, Vector_Scalar(Vector_Normalize((Vector) { randX, randY }), 64));
            if (Terrain_LineOfSight(terrain, motion->pos, newTarget, motion->z)) {
                if (isPatrol) {
                    patrol->patrolPoint = newTarget;
                } else {
                    target->tar = newTarget;
                    target->lookat = newTarget;
                }
            }
        }
    }
}

void AI_OrderInfantry(Scene* scene, ComponentKey key)
{
    printf("Order infantry\n");
    system(scene, id, UNIT_COMPONENT_ID, PRODUCER_COMPONENT_ID, key)
    {
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Expansion* expansion = (Expansion*)Scene_GetComponent(scene, id, EXPANSION_COMPONENT_ID);

        if (unit->type == UnitType_ACADEMY && producer->orderTicksRemaining < 0) {
            Match_PlaceOrder(scene, nation, producer, expansion, UnitType_INFANTRY);
        }
    }
}

void AI_OrderCavalry(Scene* scene, ComponentKey key)
{
    printf("Order cavalry\n");
}

// Todo: Spiral search
void AI_BuildCity(Scene* scene, ComponentKey key)
{
    printf("Build city\n");

    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID, key)
    {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);

        float tempDistance = FLT_MAX;
        Vector tempTarget = { -1, -1 };
        for (int y = 0; y < terrain->tileSize; y++) {
            for (int x = 0; x < terrain->tileSize; x++) {
                if (Terrain_GetBuildingAt(terrain, x * 64 + 32, y * 64 + 32) != INVALID_ENTITY_INDEX)
                    continue;
                Vector point = { x * 64.0f + 32.0f, y * 64.0f + 32.0f };
                // Find if there is a city with cabdist less than 3 tiles
                bool exitFlag = false;
                system(scene, cityID, CITY_COMPONENT_ID)
                {
                    Motion* cityMotion = (Motion*)Scene_GetComponent(scene, cityID, MOTION_COMPONENT_ID);
                    if (Vector_CabDist(point, cityMotion->pos) < 3 * 64) {
                        exitFlag = true;
                        break;
                    }
                }
                if (exitFlag) {
                    continue;
                }
                if (Terrain_GetHeight(terrain, (int)point.x, (int)point.y) < 0.5)
                    continue;
                float distance = Vector_Dist(motion->pos, point) - Terrain_GetHeight(terrain, (int)point.x, (int)point.y) * 10;
                float northHeight = Terrain_GetHeight(terrain, (int)point.x, (int)point.y - 64);
                float eastHeight = Terrain_GetHeight(terrain, (int)point.x + 64, (int)point.y);
                float southHeight = Terrain_GetHeight(terrain, (int)point.x, (int)point.y + 64);
                float westHeight = Terrain_GetHeight(terrain, (int)point.x - 64, (int)point.y);
                if ((northHeight < 0.5 && northHeight != -1) || (eastHeight < 0.5 && eastHeight != -1) || (southHeight < 0.5 && southHeight != -1) || (westHeight < 0.5 && westHeight != -1)) {
                    distance /= 10;
                }
                if (distance > tempDistance)
                    continue;
                if (!Terrain_LineOfSight(terrain, motion->pos, point, 0.5))
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
        }
        break;
    }
}

static void findExpansionSpot(Scene* scene, ComponentKey key, UnitType type)
{
    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID, key)
    {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);

        float tempDistance = FLT_MAX;
        Vector tempTarget = (Vector) { -1, -1 };
        for (int y = 0; y < terrain->tileSize; y++) {
            for (int x = 0; x < terrain->tileSize; x++) {
                if (Terrain_GetBuildingAt(terrain, x * 64 + 32, y * 64 + 32) != INVALID_ENTITY_INDEX)
                    continue;

                // Only go to squares near friendly cities
                Vector point = { x * 64.0f + 32.0f, y * 64.0f + 32.0f };
                bool foundFlag = false;
                system(scene, cityID, CITY_COMPONENT_ID, key)
                {
                    Motion* cityMotion = (Motion*)Scene_GetComponent(scene, cityID, MOTION_COMPONENT_ID);
                    City* homeCity = (City*)Scene_GetComponent(scene, cityID, CITY_COMPONENT_ID);

                    if (Vector_CabDist(point, cityMotion->pos) == 64) {
                        foundFlag = true;
                        break;
                    }
                }
                if (!foundFlag)
                    continue;

                if (Terrain_GetHeight(terrain, (int)point.x, (int)point.y) < 0.5)
                    continue;

                float distance = Vector_Dist(motion->pos, point);
                if (type == UnitType_MINE) {
                    distance *= Terrain_GetOre(terrain, point.x, point.y);
                }

                if (distance > tempDistance)
                    continue;

                if (!Terrain_LineOfSight(terrain, motion->pos, point, 0.5))
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
                Match_BuyExpansion(scene, type, simpleRenderable->nation, motion->pos);
            }
        }
    }
}

void AI_BuildMine(Scene* scene, ComponentKey key)
{
    printf("Build mine\n");
    findExpansionSpot(scene, key, UnitType_MINE);
}

void AI_BuildFactory(Scene* scene, ComponentKey key)
{
    printf("Build factory\n");
    findExpansionSpot(scene, key, UnitType_FACTORY);
}

void AI_BuildFarm(Scene* scene, ComponentKey key)
{
    printf("Build farm\n");
    findExpansionSpot(scene, key, UnitType_FARM);
}

void AI_BuildAcademy(Scene* scene, ComponentKey key)
{
    printf("Build academy\n");
    findExpansionSpot(scene, key, UnitType_ACADEMY);
}

void AI_Init(Goap* goap)
{
    goap->updateVariableSystem = &AI_UpdateVariables;
    Goap_AddAction(goap, NULL, 0, HAS_WON, 3, NO_KNOWN_ENEMY_UNITS, FOUND_ENEMY_CAPITAL, COMBATANTS_AT_ENEMY_CAPITAL);

    Goap_AddAction(goap, &AI_TargetGroundUnitsRandomly, 1, FOUND_ENEMY_CAPITAL, 1, HAS_CAVALRY);
    Goap_AddAction(goap, &AI_TargetGroundUnitsRandomly, 1, FOUND_ENEMY_CAPITAL, 1, HAS_INFANTRY);

    Goap_AddAction(goap, &AI_OrderInfantry, 1, HAS_INFANTRY, 3, AFFORD_INFANTRY_COINS, HAS_AVAILABLE_ACADEMY, HAS_POPULATION);
    Goap_AddAction(goap, &AI_OrderCavalry, 1, HAS_CAVALRY, 4, AFFORD_CAVALRY_COINS, AFFORD_CAVALRY_ORE, HAS_AVAILABLE_FACTORY, HAS_POPULATION);

    // Engineer build actions
    Goap_AddAction(goap, &AI_BuildCity, 1, HAS_COINS, 2, AFFORD_CITY_COINS, ENGINEER_ISNT_BUSY);
    Goap_AddAction(goap, &AI_BuildFactory, 1, HAS_AVAILABLE_FACTORY, 5, AFFORD_FACTORY_COINS, ENGINEER_ISNT_BUSY, HAS_COINS, HAS_ORE, HAS_POPULATION);
    Goap_AddAction(goap, &AI_BuildAcademy, 1, HAS_AVAILABLE_ACADEMY, 4, AFFORD_ACADEMY_COINS, ENGINEER_ISNT_BUSY, HAS_COINS, HAS_POPULATION);
    Goap_AddAction(goap, &AI_BuildMine, 1, HAS_ORE, 4, AFFORD_MINE_COINS, ENGINEER_ISNT_BUSY, HAS_COINS, HAS_POPULATION);
    Goap_AddAction(goap, &AI_BuildFarm, 1, HAS_POPULATION, 3, AFFORD_FARM_COINS, ENGINEER_ISNT_BUSY, HAS_COINS);
}