#pragma once
#include "ai.h";
#include "../components/components.h"
#include "../engine/gameState.h"
#include "../util/debug.h"
#include "./match.h"
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

static void findExpansionSpot(Scene* scene, ComponentKey key, UnitType type, bool leaveOneSpace)
{
    Nation* nation = NULL;
    system(scene, id, NATION_COMPONENT_ID, key)
    {
        nation = (Nation*)Scene_GetComponent(scene, id, NATION_COMPONENT_ID);
        break;
    }
    if (nation == NULL) {
        PANIC("Didnt find nation");
    }

    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID, key)
    {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);

        float tempDistance = FLT_MAX;
        Vector tempTarget = (Vector) { -1, -1 };
        for (int i = 0; i < nation->cities->size; i++) {
            EntityID cityID = *(EntityID*)Arraylist_Get(nation->cities, i);

            Motion* cityMotion = (Motion*)Scene_GetComponent(scene, cityID, MOTION_COMPONENT_ID);
            City* homeCity = (City*)Scene_GetComponent(scene, cityID, CITY_COMPONENT_ID);

            // Only build airfields at cities that don't have airfields and do have factories
            if (type == UnitType_AIRFIELD && (Match_CityHasType(scene, homeCity, UnitType_AIRFIELD) || !Match_CityHasType(scene, homeCity, UnitType_FACTORY))) {
                continue;
            }

            int remaningSpaces = 0;
            if (leaveOneSpace) {
                for (int y = -64; y <= 64; y += 64) {
                    for (int x = -64; x <= 64; x += 64) {
                        Vector point = (Vector) { x + cityMotion->pos.x, y + cityMotion->pos.y };
                        if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                            continue;

                        // Only go to squares adjacent friendly cities
                        if (Vector_CabDist(point, cityMotion->pos) != 64)
                            continue;

                        if (Terrain_GetHeight(terrain, (int)point.x, (int)point.y) < 0.5)
                            continue;
                        remaningSpaces++;
                    }
                }
                if (remaningSpaces <= 1) {
                    continue;
                }
            }

            // Search around city
            for (int y = -64; y <= 64; y += 64) {
                for (int x = -64; x <= 64; x += 64) {
                    Vector point = (Vector) { x + cityMotion->pos.x, y + cityMotion->pos.y };
                    if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                        continue;

                    // Only go to squares adjacent friendly cities
                    if (Vector_CabDist(point, cityMotion->pos) != 64)
                        continue;

                    if (Terrain_GetHeight(terrain, (int)point.x, (int)point.y) < 0.5)
                        continue;

                    float distance = Vector_Dist(motion->pos, point);
                    // If youre looking to build a mine, find the best place, regardless of distance
                    if (type == UnitType_MINE) {
                        distance = 1.0f / Terrain_GetOre(terrain, point.x, point.y);
                    }

                    if (distance > tempDistance)
                        continue;

                    if (!Terrain_LineOfSight(terrain, motion->pos, point, 0.5))
                        continue;
                    tempTarget = point;
                    tempDistance = distance;
                }
            }
        }

        // Factory point found, build expansion
        if (tempTarget.x != -1) {
            target->tar = tempTarget;
            target->lookat = tempTarget;
            if (Vector_Dist(motion->pos, target->tar) < 32) {
                Match_BuyExpansion(scene, type, simpleRenderable->nation, motion->pos);
            }
        }
    }
}

static void findPortTile(Scene* scene, ComponentKey key)
{
    Nation* nation = NULL;
    system(scene, id, NATION_COMPONENT_ID, key)
    {
        nation = (Nation*)Scene_GetComponent(scene, id, NATION_COMPONENT_ID);
        break;
    }
    if (nation == NULL) {
        PANIC("Didnt find nation");
    }

    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID, key)
    {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);

        float tempDistance = FLT_MAX;
        Vector tempTarget = (Vector) { -1, -1 };
        for (int i = 0; i < nation->cities->size; i++) {
            EntityID cityID = *(EntityID*)Arraylist_Get(nation->cities, i);

            Motion* cityMotion = (Motion*)Scene_GetComponent(scene, cityID, MOTION_COMPONENT_ID);
            City* homeCity = (City*)Scene_GetComponent(scene, cityID, CITY_COMPONENT_ID);

            // Search port tiles
            for (int j = 0; j < terrain->ports->size; j++) {
                Vector point = *(Vector*)Arraylist_Get(terrain->ports, j);
                if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                    continue;

                // Only go to squares adjacent friendly cities
                if (Vector_CabDist(point, cityMotion->pos) != 64)
                    continue;

                float distance = Vector_Dist(motion->pos, point);

                if (distance > tempDistance)
                    continue;

                if (!Terrain_LineOfSight(terrain, motion->pos, point, 0.5))
                    continue;
                tempTarget = point;
                tempDistance = distance;
            }
        }

        // Factory point found, build expansion
        if (tempTarget.x != -1) {
            target->tar = tempTarget;
            target->lookat = tempTarget;
            if (Vector_Dist(motion->pos, target->tar) < 32) {
                Match_BuyExpansion(scene, UnitType_PORT, simpleRenderable->nation, motion->pos);
                target->tar = motion->pos;
                target->lookat = motion->pos;
            }
        }
    }
}

static void orderFromProducer(Scene* scene, ComponentKey key, UnitType producerType, UnitType orderType)
{
    system(scene, id, UNIT_COMPONENT_ID, PRODUCER_COMPONENT_ID, key)
    {
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Expansion* expansion = (Expansion*)Scene_GetComponent(scene, id, EXPANSION_COMPONENT_ID);

        if (orderType == UnitType_FIGHTER || orderType == UnitType_ATTACKER || orderType == UnitType_BOMBER) {
            City* homeCity = (City*)Scene_GetComponent(scene, expansion->homeCity, CITY_COMPONENT_ID);
            if (!Match_CityHasType(scene, homeCity, UnitType_AIRFIELD)) {
                continue;
            }
        }
        if (unit->type == producerType && producer->orderTicksRemaining < 0) {
            Match_PlaceOrder(scene, nation, producer, expansion, orderType);
        }
    }
}

void AI_UpdateVariables(Scene* scene, Goap* goap, ComponentKey key)
{
    system(scene, id, NATION_COMPONENT_ID, key)
    {
        Nation* nation = (Nation*)Scene_GetComponent(scene, id, NATION_COMPONENT_ID);
        Nation* enemyNation = (Nation*)Scene_GetComponent(scene, nation->enemyNation, NATION_COMPONENT_ID);
        Unit* enemyCapitalUnit = (Unit*)Scene_GetComponent(scene, nation->capital, UNIT_COMPONENT_ID);

        int knownEnemies = 0; // ground units
        int knownEnemySeaUnits = 0;
        int knownEnemyPlanes = 0; // air units
        system(scene, otherID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID, nation->enemyNationFlag)
        {
            Unit* unit = (Unit*)Scene_GetComponent(scene, otherID, UNIT_COMPONENT_ID);
            if (unit->knownByEnemy) {
                if (unit->type == UnitType_FIGHTER || unit->type == UnitType_ATTACKER || unit->type == UnitType_BOMBER) {
                    knownEnemyPlanes++;
                } else if (unit->type == UnitType_DESTROYER || unit->type == UnitType_CRUISER || unit->type == UnitType_BATTLESHIP) {
                    knownEnemySeaUnits++;
                } else {
                    knownEnemies++;
                }
            }
        }

        // How many ticks does it take to make an ore
        const float averageTicksPerOreMade = nation->unitCount[UnitType_MINE] == 0 ? 54000.0f : 2 * ticksPerLabor / (nation->unitCount[UnitType_MINE]);
        // How many ticks does it take to use an ore
        const float averageTicksPerOreUsed = nation->unitCount[UnitType_FACTORY] == 0 ? 54000.0f : (ticksPerLabor * 15.0f) / (5.0f * nation->unitCount[UnitType_FACTORY]);
        // Build a mine if it takes more ticks to make an ore than it does to use one (Different from coins below)

        // How many ticks does it take to make a coin
        const float averageTicksPerCoinMade = nation->unitCount[UnitType_CITY] == 0 ? 54000.0f : ticksPerLabor / nation->unitCount[UnitType_CITY];
        // How many ticks does it take to use a coin
        const int coinUsers = nation->unitCount[UnitType_FACTORY] + nation->unitCount[UnitType_ACADEMY] + nation->unitCount[UnitType_PORT];
        const float averageTicksPerCoinUsed = coinUsers == 0 ? 54000.0f : (ticksPerLabor * 22.0f) / (15.0f * coinUsers);
        // Build a factory if it takes less ticks to make a coin than it does to use one (Different from ore above)
        // makeCoinTicks < useCoinTicks

        goap->variables[COMBATANTS_AT_ENEMY_CAPITAL] = false;

        goap->variables[NO_KNOWN_ENEMY_UNITS] = knownEnemies == 0;
        goap->variables[FOUND_ENEMY_CAPITAL] = enemyCapitalUnit->knownByEnemy;
        goap->variables[SEA_SUPREMACY] = nation->unitCount[UnitType_DESTROYER] > max(1, knownEnemySeaUnits);

        goap->variables[HAS_FIGHTER] = nation->unitCount[UnitType_FIGHTER] + nation->prodCount[UnitType_FIGHTER] > knownEnemyPlanes;
        goap->variables[HAS_ATTACKER] = nation->unitCount[UnitType_ATTACKER] + nation->prodCount[UnitType_ATTACKER] > knownEnemies;
        goap->variables[HAS_COINS] = averageTicksPerCoinMade < averageTicksPerCoinUsed;
        goap->variables[HAS_ORE] = averageTicksPerOreMade < averageTicksPerOreUsed;
        goap->variables[HAS_POPULATION] = nation->resources[ResourceType_POPULATION] < nation->resources[ResourceType_POPULATION_CAPACITY];

        goap->variables[AFFORD_INFANTRY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_INFANTRY];
        goap->variables[AFFORD_CAVALRY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_CAVALRY];
        goap->variables[AFFORD_CAVALRY_ORE] = nation->resources[ResourceType_ORE] >= nation->costs[ResourceType_ORE][UnitType_CAVALRY];
        goap->variables[AFFORD_FIGHTER_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FIGHTER];
        goap->variables[AFFORD_FIGHTER_ORE] = nation->resources[ResourceType_ORE] >= nation->costs[ResourceType_ORE][UnitType_FIGHTER];
        goap->variables[AFFORD_ATTACKER_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_ATTACKER];
        goap->variables[AFFORD_ATTACKER_ORE] = nation->resources[ResourceType_ORE] >= nation->costs[ResourceType_ORE][UnitType_ATTACKER];

        goap->variables[AFFORD_CITY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_CITY];
        goap->variables[AFFORD_MINE_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_MINE];
        goap->variables[AFFORD_FACTORY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FACTORY];
        goap->variables[AFFORD_AIRFIELD_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_AIRFIELD];
        goap->variables[AFFORD_FARM_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FARM];
        goap->variables[AFFORD_ACADEMY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_ACADEMY];

        goap->variables[ENGINEER_ISNT_BUSY] = false;
        goap->variables[HAS_ENGINEER] = nation->unitCount[UnitType_ENGINEER] + nation->prodCount[UnitType_ENGINEER] > 0;
        Motion* engineerMotion = NULL;
        system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID, key)
        {
            Motion* motion = (Target*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
            Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

            if (Vector_Dist(motion->pos, target->tar) < 32) {
                goap->variables[ENGINEER_ISNT_BUSY] = true;
                engineerMotion = motion;
                break;
            }
        }

        goap->variables[SPACE_FOR_AIRFIELD] = false;
        goap->variables[SPACE_FOR_EXPANSION] = false;
        goap->variables[SPACE_FOR_TWO_EXPANSIONS] = false;
        goap->variables[SPACE_FOR_PORT] = false;
        if (engineerMotion != NULL) {
            for (int i = 0; i < nation->cities->size; i++) {
                EntityID cityID = *(EntityID*)Arraylist_Get(nation->cities, i);
                Motion* cityMotion = (Motion*)Scene_GetComponent(scene, cityID, MOTION_COMPONENT_ID);
                City* homeCity = (City*)Scene_GetComponent(scene, cityID, CITY_COMPONENT_ID);

                int remainingSpaces = 0;
                for (int x = -64; x <= 64; x += 64) {
                    for (int y = -64; y <= 64; y += 64) {
                        Vector point = { x + cityMotion->pos.x, y + cityMotion->pos.y };
                        if (Vector_CabDist(point, cityMotion->pos) == 64 && Terrain_GetHeightForBuilding(terrain, point.x, point.y) > 0.5f && Terrain_GetBuildingAt(terrain, point.x, point.y) == INVALID_ENTITY_INDEX && Terrain_LineOfSight(terrain, engineerMotion->pos, point, 0.5f)) {
                            if (!Match_CityHasType(scene, homeCity, UnitType_AIRFIELD) && Match_CityHasType(scene, homeCity, UnitType_FACTORY)) {
                                goap->variables[SPACE_FOR_AIRFIELD] = true;
                            }
                            goap->variables[SPACE_FOR_EXPANSION] = true;
                            remainingSpaces++;
                        }
                    }
                }
                if (remainingSpaces >= 1) {
                    goap->variables[SPACE_FOR_TWO_EXPANSIONS] = true;
                }
                for (int j = 0; j < terrain->ports->size; j++) {
                    Vector point = *(Vector*)Arraylist_Get(terrain->ports, j);
                    if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                        continue;

                    // Only go to squares adjacent friendly cities
                    if (Vector_CabDist(point, cityMotion->pos) != 64)
                        continue;

                    float distance = Vector_Dist(engineerMotion->pos, point);

                    if (!Terrain_LineOfSight(terrain, engineerMotion->pos, point, 0.5))
                        continue;
                    goap->variables[SPACE_FOR_PORT] = true;
                }
            }
        }

        // Update has available variables
        goap->variables[HAS_AVAILABLE_FACTORY] = false;
        goap->variables[HAS_AVAILABLE_AIRFIELD] = false;
        goap->variables[HAS_AVAILABLE_ACADEMY] = false;
        system(scene, id, UNIT_COMPONENT_ID, PRODUCER_COMPONENT_ID, key)
        {
            Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
            Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
            Expansion* expansion = (Expansion*)Scene_GetComponent(scene, id, EXPANSION_COMPONENT_ID);

            if (producer->orderTicksRemaining < 0) {
                switch (unit->type) {
                case UnitType_FACTORY:
                    goap->variables[HAS_AVAILABLE_FACTORY] = true;
                    City* homeCity = (City*)Scene_GetComponent(scene, expansion->homeCity, CITY_COMPONENT_ID);
                    if (Match_CityHasType(scene, homeCity, UnitType_AIRFIELD)) {
                        goap->variables[HAS_AVAILABLE_AIRFIELD] = true;
                    }
                    break;
                case UnitType_ACADEMY:
                    goap->variables[HAS_AVAILABLE_ACADEMY] = true;
                    break;
                }
            }
        }

        /*
        goap->variables[HAS_CAVALRY] = nation->unitCount[UnitType_CAVALRY] + nation->prodCount[UnitType_CAVALRY] > knownEnemies;
        goap->variables[HAS_INFANTRY] = nation->unitCount[UnitType_INFANTRY] + nation->prodCount[UnitType_INFANTRY] > knownEnemies;
		*/
    }
}

// Really only for tactical planning, let GOAP handle strategic stuff, it's good at that
void AI_TargetGroundUnitsRandomly(Scene* scene, ComponentKey key)
{
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

        // Search through alerted tiles, go to closest one
        bool foundEnemy = false;
        Vector closestTile = { -1, -1 };
        float tempDist = FLT_MAX;
        for (int i = 0; i < nation->highPrioritySpaces->size; i++) {
            Vector point = *(Vector*)Arraylist_Get(nation->highPrioritySpaces, i);
            Vector scaledUp = Vector_Scalar(point, 32);
            float distance = Vector_Dist(motion->pos, scaledUp);
            if (distance < tempDist && Terrain_LineOfSight(terrain, motion->pos, scaledUp, motion->z)) {
                tempDist = distance;
                closestTile = scaledUp;
                foundEnemy = true;
            }
        }

        // Spiral search for spaces to scout for
        int xOffsets[] = { 1, 0, -1, 0 };
        int yOffsets[] = { 0, 1, 0, -1 };
        int x = 0;
        int y = 0;
        int r = rand() % 4;
        int dx = xOffsets[r];
        int dy = yOffsets[r];
        for (int i = 0; !foundEnemy && i < nation->visitedSpacesSize * nation->visitedSpacesSize; i++) {
            Vector point = { x + (int)(motion->pos.x / 32), y + (int)(motion->pos.y / 32) };
            if (point.x >= 0 && point.y >= 0 && point.x < nation->visitedSpacesSize && point.y < nation->visitedSpacesSize) {
                float spaceValue = nation->visitedSpaces[(int)point.x + (int)(point.y) * nation->visitedSpacesSize];
                // If there is an enemy
                if (spaceValue <= 0) {
                    Vector newPoint = Vector_Scalar(point, 32);
                    float newDist = Vector_Dist(motion->pos, newPoint);
                    // If near enough to target, or if new target is closer
                    if ((dist < 3 || newDist < dist) && Terrain_LineOfSight(terrain, motion->pos, newPoint, motion->z)) {
                        closestTile = newPoint;
                        foundEnemy = true; // Breaks out of loop
                    }
                }
            }
            // Update spiral
            if (x == y || (x < 0 && x == -y) || (x > 0 && x == 1 - y)) {
                int temp = dx;
                dx = -dy;
                dy = temp;
            }
            x += dx;
            y += dy;
        }

        if (foundEnemy) {
            if (isPatrol) {
                patrol->patrolPoint = closestTile;
            } else {
                target->tar = closestTile;
                target->lookat = closestTile;
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

void AI_TargetEnemyCapital(Scene* scene, ComponentKey key)
{
    system(scene, id, MOTION_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID, key)
    {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);
        Nation* enemyNation = (Nation*)Scene_GetComponent(scene, nation->enemyNation, NATION_COMPONENT_ID);
        Motion* enemyCapital = (Motion*)Scene_GetComponent(scene, enemyNation->capital, MOTION_COMPONENT_ID);

        if (Terrain_LineOfSight(terrain, motion->pos, enemyCapital->pos, motion->z)) {
            target->tar = enemyCapital->pos;
            target->lookat = enemyCapital->pos;
        }
    }
}

void AI_OrderInfantry(Scene* scene, ComponentKey key)
{
    orderFromProducer(scene, key, UnitType_ACADEMY, UnitType_INFANTRY);
}

void AI_OrderCavalry(Scene* scene, ComponentKey key)
{
    if (rand() % 2 == 0) {
        orderFromProducer(scene, key, UnitType_FACTORY, UnitType_CAVALRY);
    } else {
        orderFromProducer(scene, key, UnitType_FACTORY, UnitType_ARTILLERY);
    }
}

void AI_OrderFighter(Scene* scene, ComponentKey key)
{
    orderFromProducer(scene, key, UnitType_FACTORY, UnitType_FIGHTER);
}

void AI_OrderAttacker(Scene* scene, ComponentKey key)
{
    orderFromProducer(scene, key, UnitType_FACTORY, UnitType_ATTACKER);
}

void AI_OrderEngineer(Scene* scene, ComponentKey key)
{
    orderFromProducer(scene, key, UnitType_ACADEMY, UnitType_ENGINEER);
}

void AI_BuildCity(Scene* scene, ComponentKey key)
{
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
                if (Terrain_GetOre(terrain, (int)point.x, (int)point.y) < 0.5)
                    continue;
                float distance = Vector_Dist(motion->pos, point) - Terrain_GetHeight(terrain, (int)point.x, (int)point.y) * 10;
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

void AI_BuildPortCity(Scene* scene, ComponentKey key)
{
    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID, key)
    {
        Motion* motion = (Motion*)Scene_GetComponent(scene, id, MOTION_COMPONENT_ID);
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        SimpleRenderable* simpleRenderable = (SimpleRenderable*)Scene_GetComponent(scene, id, SIMPLE_RENDERABLE_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Nation* nation = (Nation*)Scene_GetComponent(scene, simpleRenderable->nation, NATION_COMPONENT_ID);

        float tempDistance = FLT_MAX;
        Vector tempTarget = { -1, -1 };
        for (int i = 0; i < terrain->ports->size; i++) {
            Vector portVector = *(Vector*)Arraylist_Get(terrain->ports, i);
            for (int y = -64; y <= 64; y += 64) {
                for (int x = -64; x <= 64; x += 64) {
                    Vector point = Vector_Add(portVector, (Vector) { x, y });
                    if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                        continue;
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
                    if (Terrain_GetOre(terrain, (int)point.x, (int)point.y) < 0.5)
                        continue;
                    float distance = Vector_Dist(motion->pos, point) - Terrain_GetHeight(terrain, (int)point.x, (int)point.y) * 10;
                    if (distance > tempDistance)
                        continue;
                    if (!Terrain_LineOfSight(terrain, motion->pos, point, 0.5))
                        continue;
                    tempTarget = point;
                    tempDistance = distance;
                }
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

void AI_BuildMine(Scene* scene, ComponentKey key)
{
    findExpansionSpot(scene, key, UnitType_MINE, false);
}

void AI_BuildFactory(Scene* scene, ComponentKey key)
{
    findExpansionSpot(scene, key, UnitType_FACTORY, false);
}

void AI_BuildFactoryForAirfield(Scene* scene, ComponentKey key)
{
    findExpansionSpot(scene, key, UnitType_FACTORY, true);
}

void AI_BuildPort(Scene* scene, ComponentKey key)
{
    findExpansionSpot(scene, key, UnitType_PORT, false);
}

void AI_BuildAirfield(Scene* scene, ComponentKey key)
{
    findExpansionSpot(scene, key, UnitType_AIRFIELD, false);
}

void AI_BuildFarm(Scene* scene, ComponentKey key)
{
    findExpansionSpot(scene, key, UnitType_FARM, false);
}

void AI_BuildAcademy(Scene* scene, ComponentKey key)
{
    findExpansionSpot(scene, key, UnitType_ACADEMY, false);
}

void AI_Init(Goap* goap)
{
    goap->updateVariableSystem = &AI_UpdateVariables;
    Goap_AddAction(goap, "Win", NULL, HAS_WON, 1, COMBATANTS_AT_ENEMY_CAPITAL, 1);

    Goap_AddAction(goap, "Target capital", &AI_TargetEnemyCapital, COMBATANTS_AT_ENEMY_CAPITAL, 4, NO_KNOWN_ENEMY_UNITS, SEA_SUPREMACY, FOUND_ENEMY_CAPITAL, 1, 2, 3, 4);

    // If there are enemies, order units
    Goap_AddAction(goap, "Rand target", &AI_TargetGroundUnitsRandomly, NO_KNOWN_ENEMY_UNITS, 0, 0);
    Goap_AddAction(goap, "Rand target", &AI_TargetGroundUnitsRandomly, NO_KNOWN_ENEMY_UNITS, 2, HAS_ENGINEER, HAS_INFANTRY, 1, 3);
    Goap_AddAction(goap, "Rand target", &AI_TargetGroundUnitsRandomly, NO_KNOWN_ENEMY_UNITS, 2, HAS_ENGINEER, HAS_CAVALRY, 1, 2);
    Goap_AddAction(goap, "Rand target", &AI_TargetGroundUnitsRandomly, NO_KNOWN_ENEMY_UNITS, 2, HAS_ENGINEER, HAS_ATTACKER, 1, 1);

    // If you need to find the enemy capital
    // If you can build these things, then do so. Otherwise, set units targets
    Goap_AddAction(goap, "Rand target", &AI_TargetGroundUnitsRandomly, FOUND_ENEMY_CAPITAL, 0, 0);
    Goap_AddAction(goap, "Rand target", &AI_TargetGroundUnitsRandomly, FOUND_ENEMY_CAPITAL, 2, HAS_ENGINEER, HAS_INFANTRY, 1, 3);
    Goap_AddAction(goap, "Rand target", &AI_TargetGroundUnitsRandomly, FOUND_ENEMY_CAPITAL, 2, HAS_ENGINEER, HAS_CAVALRY, 1, 2);
    Goap_AddAction(goap, "Rand target", &AI_TargetGroundUnitsRandomly, FOUND_ENEMY_CAPITAL, 2, HAS_ENGINEER, HAS_ATTACKER, 1, 1);

    Goap_AddAction(goap, "Lol idk", &AI_TargetGroundUnitsRandomly, SEA_SUPREMACY, 0, 0);
    Goap_AddAction(goap, "Lol idk", &AI_TargetGroundUnitsRandomly, SEA_SUPREMACY, 1, HAS_AVAILABLE_PORT, 1);

    // Order ground units
    Goap_AddAction(goap, "Order infantry", &AI_OrderInfantry, HAS_INFANTRY, 3, AFFORD_INFANTRY_COINS, HAS_AVAILABLE_ACADEMY, HAS_POPULATION, 1, 1, 1);
    Goap_AddAction(goap, "Order cavalry", &AI_OrderCavalry, HAS_CAVALRY, 4, AFFORD_CAVALRY_COINS, AFFORD_CAVALRY_ORE, HAS_AVAILABLE_FACTORY, HAS_POPULATION, 1, 1, 1, 1);
    Goap_AddAction(goap, "Order engineer", &AI_OrderEngineer, HAS_ENGINEER, 3, AFFORD_ENGINEER_COINS, HAS_AVAILABLE_ACADEMY, HAS_POPULATION, 1, 1, 1);

    // Order planes
    Goap_AddAction(goap, "Order fighter", &AI_OrderFighter, HAS_FIGHTER, 4, AFFORD_FIGHTER_COINS, AFFORD_FIGHTER_ORE, HAS_AVAILABLE_AIRFIELD, HAS_POPULATION, 1, 1, 1, 1);
    Goap_AddAction(goap, "Order attacker", &AI_OrderAttacker, HAS_ATTACKER, 5, AFFORD_ATTACKER_COINS, AFFORD_ATTACKER_ORE, HAS_FIGHTER, HAS_AVAILABLE_AIRFIELD, HAS_POPULATION, 1, 1, 1, 1, 1);

    // Engineer build actions
    Goap_AddAction(goap, "City for coins", &AI_BuildCity, HAS_COINS, 2, AFFORD_CITY_COINS, ENGINEER_ISNT_BUSY, 1, 1);
    Goap_AddAction(goap, "city for expan", &AI_BuildCity, SPACE_FOR_EXPANSION, 2, AFFORD_CITY_COINS, ENGINEER_ISNT_BUSY, 1, 1);
    Goap_AddAction(goap, "city for 2expan", &AI_BuildCity, SPACE_FOR_TWO_EXPANSIONS, 2, AFFORD_CITY_COINS, ENGINEER_ISNT_BUSY, 1, 1);
    Goap_AddAction(goap, "city for port", &AI_BuildPortCity, SPACE_FOR_PORT, 2, AFFORD_CITY_COINS, ENGINEER_ISNT_BUSY, 1, 1);
    Goap_AddAction(goap, "Build factory", &AI_BuildFactory, HAS_AVAILABLE_FACTORY, 5, SPACE_FOR_EXPANSION, AFFORD_FACTORY_COINS, HAS_COINS, HAS_ORE, HAS_POPULATION, 1, 1, 1, 1, 1);
    Goap_AddAction(goap, "Build port", &AI_BuildPort, HAS_AVAILABLE_PORT, 5, SPACE_FOR_PORT, ENGINEER_CAN_SEE_PORT_TILE, AFFORD_PORT_COINS, HAS_COINS, HAS_ORE, HAS_POPULATION, 1, 1, 1, 1, 1);
    Goap_AddAction(goap, "factry for air", &AI_BuildFactoryForAirfield, HAS_AVAILABLE_AIRFIELD, 5, SPACE_FOR_TWO_EXPANSIONS, AFFORD_FACTORY_COINS, HAS_COINS, HAS_ORE, HAS_POPULATION, 1, 1, 1, 1, 1);
    Goap_AddAction(goap, "Build airfield", &AI_BuildAirfield, HAS_AVAILABLE_AIRFIELD, 5, SPACE_FOR_EXPANSION, SPACE_FOR_AIRFIELD, AFFORD_AIRFIELD_COINS, HAS_AVAILABLE_FACTORY, HAS_POPULATION, 1, 1, 1, 1, 1);
    Goap_AddAction(goap, "Build academy", &AI_BuildAcademy, HAS_AVAILABLE_ACADEMY, 4, SPACE_FOR_EXPANSION, AFFORD_ACADEMY_COINS, HAS_COINS, HAS_POPULATION, 1, 1, 1, 1);
    Goap_AddAction(goap, "Build mine", &AI_BuildMine, HAS_ORE, 3, SPACE_FOR_EXPANSION, AFFORD_MINE_COINS, HAS_POPULATION, 1, 1, 1);
    Goap_AddAction(goap, "Build farm", &AI_BuildFarm, HAS_POPULATION, 3, SPACE_FOR_EXPANSION, AFFORD_FARM_COINS, 1, 1);
}