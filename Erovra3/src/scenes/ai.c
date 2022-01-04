#pragma once
#include "ai.h"
#include "../engine/apricot.h"
#include "../assemblages/components.h"
#include "../util/debug.h"
#include "./match.h"
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

/*
@param leaveOneSpace	Leave at least one space, so that you can build something like an airfield
*/
static void findExpansionSpot(Scene* scene, Nation* nation, UnitType type, bool leaveOneSpace)
{
    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        if (sprite->nation != nation) {
            continue;
        }
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

        float tempDistance = FLT_MAX;
        Vector tempTarget = (Vector) { -1, -1 };
        for (int i = 0; i < nation->cities->size; i++) {
            EntityID cityID = *(EntityID*)Arraylist_Get(nation->cities, i);

            Sprite* citySprite = (Sprite*)Scene_GetComponent(scene, cityID, SPRITE_COMPONENT_ID);
            City* homeCity = (City*)Scene_GetComponent(scene, cityID, CITY_COMPONENT_ID);

            // Only build airfields at cities that don't have airfields and do have factories
            if (type == UnitType_AIRFIELD && (Match_CityHasType(scene, homeCity, UnitType_AIRFIELD) || !Match_CityHasType(scene, homeCity, UnitType_FACTORY))) {
                continue;
            }

            int remaningSpaces = 0;
            if (leaveOneSpace) {
                for (int y = -64; y <= 64; y += 64) {
                    for (int x = -64; x <= 64; x += 64) {
                        Vector point = (Vector) { x + citySprite->pos.x, y + citySprite->pos.y };
                        if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                            continue;

                        // Only go to squares adjacent friendly cities
                        if (Vector_CabDist(point, citySprite->pos) != 64)
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
            for (int y = 64; y >= -64; y -= 64) {
                for (int x = -64; x <= 64; x += 64) {
                    Vector point = (Vector) { x + citySprite->pos.x, y + citySprite->pos.y };
                    if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                        continue;

                    // Only go to squares adjacent friendly cities
                    if (Vector_CabDist(point, citySprite->pos) != 64)
                        continue;

                    if (Terrain_GetHeight(terrain, (int)point.x, (int)point.y) < 0.5)
                        continue;

                    float distance = Vector_Dist(sprite->pos, point);
                    // If youre looking to build a mine, find the best place, regardless of distance
                    if (type == UnitType_MINE) {
                        distance = 1.0f / Terrain_GetOre(terrain, point.x, point.y);
                    }

                    if (distance > tempDistance)
                        continue;

                    if (!Terrain_LineOfSight(terrain, sprite->pos, point, 0.5))
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
            if (Vector_Dist(sprite->pos, target->tar) < 32) {
                Match_BuyExpansion(scene, type, sprite->nation, sprite->pos);
            }
        }
    }
}

// Finds a good tile for a timberland unit and sends an engineer there to buy it
static void findBuildingTile(Scene* scene, Nation* nation, UnitType type)
{
    // Find an engineer for our nation
    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        if (sprite->nation != nation) {
            continue;
        }
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

        // Spiral search around engineer, find timber tile
        int xOffsets[] = { 1, 0, -1, 0 };
        int yOffsets[] = { 0, 1, 0, -1 };
        int x = 0;
        int y = 0;
        int r = rand() % 4;
        int dx = xOffsets[r];
        int dy = yOffsets[r];
        for (int i = 0; i < terrain->tileSize * terrain->tileSize; i++) {
            Vector point = { (x + (int)(sprite->pos.x / 64)) * 64 + 32, (y + (int)(sprite->pos.y / 64)) * 64 + 32 };
            // Update spiral
            if (x == y || (x < 0 && x == -y) || (x > 0 && x == 1 - y)) {
                int temp = dx;
                dx = -dy;
                dy = temp;
            }
            x += dx;
            y += dy;
            if (type == UnitType_TIMBERLAND && Terrain_GetTimber(terrain, point.x, point.y) < 0.5f) {
                continue;
            } else if (type == UnitType_MINE && Terrain_GetOre(terrain, point.x, point.y) < 0.5f) {
                continue;
            }
            if (Terrain_GetBuildingAt(terrain, point.x, point.y) == INVALID_ENTITY_INDEX && Terrain_LineOfSight(terrain, sprite->pos, point, 0.5)) {
                target->tar = point;
                target->lookat = point;
                if (Vector_CabDist(sprite->pos, target->tar) < 32) {
                    Match_BuyBuilding(scene, type, sprite->nation, sprite->pos);
                }
                return;
            }   
        }
    }
}

static void findPortTile(Scene* scene, Nation* nation)
{
    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        if (sprite->nation != nation) {
            continue;
        }
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

        float tempDistance = FLT_MAX;
        Vector tempTarget = (Vector) { -1, -1 };
        // For each city
        for (int i = 0; i < nation->cities->size; i++) {
            EntityID cityID = *(EntityID*)Arraylist_Get(nation->cities, i);

            Sprite* citySprite = (Sprite*)Scene_GetComponent(scene, cityID, SPRITE_COMPONENT_ID);
            City* homeCity = (City*)Scene_GetComponent(scene, cityID, CITY_COMPONENT_ID);

            // Search port tiles
            for (int j = 0; j < terrain->ports->size; j++) {
                Vector point = *(Vector*)Arraylist_Get(terrain->ports, j);
                if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                    continue;

                // Only go to squares adjacent friendly cities
                if (Vector_CabDist(point, citySprite->pos) != 64)
                    continue;

                float distance = Vector_Dist(sprite->pos, point);

                if (distance > tempDistance)
                    continue;

                Vector intersection = Terrain_LineOfSightPoint(terrain, citySprite->pos, point, 0.5);
                if (((int)(intersection.x / 64) + (int)(intersection.y / 64) * terrain->tileSize) == ((int)(citySprite->pos.x / 64) + (int)(citySprite->pos.y / 64) * terrain->tileSize)) {
                    continue;
                }
                // Check to see that engineer can get to port
                intersection = Terrain_LineOfSightPoint(terrain, sprite->pos, point, 0.5);
                if (((int)(intersection.x / 64) + (int)(intersection.y / 64) * terrain->tileSize) != ((int)(point.x / 64) + (int)(point.y / 64) * terrain->tileSize))
                    continue;

                tempTarget = point;
                tempDistance = distance;
            }
        }

        // Factory point found, build expansion
        if (tempTarget.x != -1) {
            target->tar = tempTarget;
            target->lookat = tempTarget;
            if (Vector_Dist(sprite->pos, target->tar) < 32) {
                Match_BuyExpansion(scene, UnitType_PORT, sprite->nation, sprite->pos);
                target->tar = sprite->pos;
                target->lookat = sprite->pos;
            }
        }
    }
}

static void orderFromProducer(Scene* scene, Nation* nation, UnitType producerType, UnitType orderType)
{
    system(scene, id, UNIT_COMPONENT_ID, PRODUCER_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        if (sprite->nation != nation) {
            continue;
        }
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, id, PRODUCER_COMPONENT_ID);
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

void AI_UpdateVariables(Scene* scene, Goap* goap, Nation* nation)
{
    int knownEnemies = 0; // ground units
    int knownEnemySeaUnits = 0;
    int knownEnemyPlanes = 0; // air units
    bool knownEnemyCapital = false;
    for (int i = 0; i < nation->enemyNations->size; i++) {
        Nation* enemyNation = *(Nation**)Arraylist_Get(nation->enemyNations, i);
        if (enemyNation->capital == INVALID_ENTITY_INDEX) {
            continue;
        }
        Unit* enemyCapitalUnit = (Unit*)Scene_GetComponent(scene, nation->capital, UNIT_COMPONENT_ID);
        knownEnemyCapital |= enemyCapitalUnit->knownByEnemy;
        system(scene, otherID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID)
        {
            Sprite* otherSprite = (Sprite*)Scene_GetComponent(scene, otherID, SPRITE_COMPONENT_ID);
            if (Arraylist_Contains(nation->enemyNations, &otherSprite->nation)) {
                continue;
            }
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
    }

    // How many ticks does it take to make an ore
    const float averageTicksPerOreMade = nation->unitCount[UnitType_MINE] == 0 ? 54000.0f : 2 * ticksPerLabor / (nation->unitCount[UnitType_MINE]);
    // How many ticks does it take to use an ore
    const float averageTicksPerOreUsed = nation->unitCount[UnitType_FACTORY] == 0 ? 54000.0f : (ticksPerLabor * 15.0f) / (15.0f * nation->unitCount[UnitType_FACTORY]);
    // Build a mine if it takes more ticks to make an ore than it does to use one (Different from coins below)

    // How many ticks does it take to make a coin
    const float averageTicksPerCoinMade = nation->unitCount[UnitType_CITY] == 0 ? 54000.0f : ticksPerLabor / nation->unitCount[UnitType_CITY];
    // How many ticks does it take to use a coin
    const int coinUsers = nation->unitCount[UnitType_FACTORY] + nation->unitCount[UnitType_ACADEMY] + nation->unitCount[UnitType_PORT];
    const float averageTicksPerCoinUsed = coinUsers == 0 ? 54000.0f : (ticksPerLabor * 22.0f) / (15.0f * coinUsers);
    // Build a factory if it takes less ticks to make a coin than it does to use one (Different from ore above)
    // makeCoinTicks < useCoinTicks

    const float averageTicksPerFoodMade = nation->unitCount[UnitType_FARM] == 0 ? 54000.0f : 1 * ticksPerLabor / (nation->unitCount[UnitType_FARM]);
    const float averageTicksPerFoodUsed = (ticksPerLabor * 6.0f) / (nation->resources[ResourceType_POPULATION]);

    goap->variables[COMBATANTS_AT_ENEMY_CAPITAL] = false;

    goap->variables[NO_KNOWN_ENEMY_UNITS] = knownEnemies == 0;
    goap->variables[FOUND_ENEMY_CAPITAL] = knownEnemyCapital;
    goap->variables[SEA_SUPREMACY] = nation->unitCount[UnitType_DESTROYER] > max(1, knownEnemySeaUnits);

    goap->variables[HAS_FIGHTER] = nation->unitCount[UnitType_FIGHTER] + nation->prodCount[UnitType_FIGHTER] >= 1;
    goap->variables[HAS_ATTACKER] = nation->unitCount[UnitType_ATTACKER] + nation->prodCount[UnitType_ATTACKER] > knownEnemies;
    goap->variables[HAS_COINS] = averageTicksPerCoinMade < averageTicksPerCoinUsed;
    goap->variables[HAS_ORE] = averageTicksPerOreMade < averageTicksPerOreUsed;
    goap->variables[HAS_FOOD] = averageTicksPerFoodMade < averageTicksPerFoodUsed;
    goap->variables[HAS_TIMBER] = nation->unitCount[UnitType_TIMBERLAND] >= 1;

    goap->variables[AFFORD_INFANTRY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_INFANTRY];
    goap->variables[AFFORD_CAVALRY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_CAVALRY];
    goap->variables[AFFORD_CAVALRY_ORE] = nation->resources[ResourceType_ORE] >= nation->costs[ResourceType_ORE][UnitType_CAVALRY];
    goap->variables[AFFORD_ENGINEER_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_ENGINEER];
    goap->variables[AFFORD_DESTROYER_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_DESTROYER];
    goap->variables[AFFORD_DESTROYER_ORE] = nation->resources[ResourceType_ORE] >= nation->costs[ResourceType_ORE][UnitType_DESTROYER];
    goap->variables[AFFORD_FIGHTER_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FIGHTER];
    goap->variables[AFFORD_FIGHTER_ORE] = nation->resources[ResourceType_ORE] >= nation->costs[ResourceType_ORE][UnitType_FIGHTER];
    goap->variables[AFFORD_ATTACKER_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_ATTACKER];
    goap->variables[AFFORD_ATTACKER_ORE] = nation->resources[ResourceType_ORE] >= nation->costs[ResourceType_ORE][UnitType_ATTACKER];

    goap->variables[AFFORD_CITY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_CITY];
    goap->variables[AFFORD_CITY_TIMBER] = nation->resources[ResourceType_TIMBER] >= nation->costs[ResourceType_TIMBER][UnitType_CITY];
    goap->variables[AFFORD_TIMBERLAND_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_TIMBERLAND];
    goap->variables[AFFORD_MINE_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_MINE];
    goap->variables[AFFORD_MINE_TIMBER] = nation->resources[ResourceType_TIMBER] >= nation->costs[ResourceType_TIMBER][UnitType_MINE];
    goap->variables[AFFORD_FACTORY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FACTORY];
    goap->variables[AFFORD_FACTORY_TIMBER] = nation->resources[ResourceType_TIMBER] >= nation->costs[ResourceType_TIMBER][UnitType_FACTORY];
    goap->variables[AFFORD_PORT_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_PORT];
    goap->variables[AFFORD_PORT_TIMBER] = nation->resources[ResourceType_TIMBER] >= nation->costs[ResourceType_TIMBER][UnitType_PORT];
    goap->variables[AFFORD_AIRFIELD_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_AIRFIELD];
    goap->variables[AFFORD_AIRFIELD_TIMBER] = nation->resources[ResourceType_TIMBER] >= nation->costs[ResourceType_TIMBER][UnitType_AIRFIELD];
    goap->variables[AFFORD_FARM_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_FARM];
    goap->variables[AFFORD_FARM_TIMBER] = nation->resources[ResourceType_TIMBER] >= nation->costs[ResourceType_TIMBER][UnitType_FARM];
    goap->variables[AFFORD_ACADEMY_COINS] = nation->resources[ResourceType_COIN] >= nation->costs[ResourceType_COIN][UnitType_ACADEMY];
    goap->variables[AFFORD_ACADEMY_TIMBER] = nation->resources[ResourceType_TIMBER] >= nation->costs[ResourceType_TIMBER][UnitType_ACADEMY];

    goap->variables[ENGINEER_ISNT_BUSY] = false;
    goap->variables[HAS_ENGINEER] = nation->unitCount[UnitType_ENGINEER] + nation->prodCount[UnitType_ENGINEER] > 0;
    Sprite* engineerSprite = NULL;
    system(scene, otherID, ENGINEER_UNIT_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, otherID, SPRITE_COMPONENT_ID);
        if (sprite->nation != nation) {
            continue;
        }
        Target* target = (Target*)Scene_GetComponent(scene, otherID, TARGET_COMPONENT_ID);

        if (Vector_Dist(sprite->pos, target->tar) < 32) {
            goap->variables[ENGINEER_ISNT_BUSY] = true;
            engineerSprite = sprite;
            break;
        }
    }

    goap->variables[SPACE_FOR_AIRFIELD] = false;
    goap->variables[SPACE_FOR_EXPANSION] = false;
    goap->variables[SPACE_FOR_TWO_EXPANSIONS] = false;
    goap->variables[SPACE_FOR_PORT] = false;
    goap->variables[HAS_PORT_TILES] = false;
    goap->variables[ENGINEER_CAN_SEE_PORT_CITY_TILE] = false;
    if (engineerSprite != NULL) {
        for (int i = 0; i < nation->cities->size; i++) {
            EntityID cityID = *(EntityID*)Arraylist_Get(nation->cities, i);
            Sprite* citySprite = (Sprite*)Scene_GetComponent(scene, cityID, SPRITE_COMPONENT_ID);
            City* homeCity = (City*)Scene_GetComponent(scene, cityID, CITY_COMPONENT_ID);

            int remainingSpaces = 0;
            for (int x = -64; x <= 64; x += 64) {
                for (int y = -64; y <= 64; y += 64) {
                    Vector point = { x + citySprite->pos.x, y + citySprite->pos.y };
                    if (Vector_CabDist(point, citySprite->pos) == 64 && Terrain_GetHeightForBuilding(terrain, point.x, point.y) > 0.5f && Terrain_GetBuildingAt(terrain, point.x, point.y) == INVALID_ENTITY_INDEX && Terrain_LineOfSight(terrain, engineerSprite->pos, point, 0.5f)) {
                        if (!Match_CityHasType(scene, homeCity, UnitType_AIRFIELD) && Match_CityHasType(scene, homeCity, UnitType_FACTORY)) {
                            goap->variables[SPACE_FOR_AIRFIELD] = true;
                        }
                        goap->variables[SPACE_FOR_EXPANSION] = true;
                        remainingSpaces++;
                    }
                }
            }
            if (remainingSpaces >= 2) {
                goap->variables[SPACE_FOR_TWO_EXPANSIONS] = true;
            }

            // Check all port tiles, see if any are adjacent to the city tile
            for (int j = 0; j < terrain->ports->size; j++) {
                goap->variables[HAS_PORT_TILES] = true;
                Vector point = *(Vector*)Arraylist_Get(terrain->ports, j);
                if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                    continue;

                // Only go to squares adjacent friendly cities
                if (Vector_CabDist(point, citySprite->pos) > 65)
                    continue;

                if (!Terrain_LineOfSight(terrain, engineerSprite->pos, citySprite->pos, 0.5))
                    continue;
                goap->variables[ENGINEER_CAN_SEE_PORT_CITY_TILE] = true;

                // Checks to see if port is buildable
                Vector intersection = Terrain_LineOfSightPoint(terrain, citySprite->pos, point, 0.5);
                if (((int)(intersection.x / 64) + (int)(intersection.y / 64) * terrain->tileSize) == ((int)(citySprite->pos.x / 64) + (int)(citySprite->pos.y / 64) * terrain->tileSize)) {
                    continue;
                }
                // Check to see that engineer can get to port
                intersection = Terrain_LineOfSightPoint(terrain, engineerSprite->pos, point, 0.5);
                if (((int)(intersection.x / 64) + (int)(intersection.y / 64) * terrain->tileSize) != ((int)(point.x / 64) + (int)(point.y / 64) * terrain->tileSize)) {
                    continue;
                }

                goap->variables[SPACE_FOR_PORT] = true;
            }
        }
    }
    goap->variables[HAS_NO_PORT_TILES] = terrain->ports->size == 0;

    // Update has available variables
    goap->variables[HAS_AVAILABLE_FACTORY] = false;
    goap->variables[HAS_AVAILABLE_AIRFIELD] = false;
    goap->variables[HAS_AVAILABLE_ACADEMY] = false;
    goap->variables[HAS_AVAILABLE_PORT] = false;
    system(scene, otherID, UNIT_COMPONENT_ID, PRODUCER_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, otherID, SPRITE_COMPONENT_ID);
        if (sprite->nation != nation) {
            continue;
        }
        Unit* unit = (Unit*)Scene_GetComponent(scene, otherID, UNIT_COMPONENT_ID);
        Producer* producer = (Producer*)Scene_GetComponent(scene, otherID, PRODUCER_COMPONENT_ID);
        Expansion* expansion = (Expansion*)Scene_GetComponent(scene, otherID, EXPANSION_COMPONENT_ID);

        if (producer->orderTicksRemaining < 0) {
            switch (unit->type) {
            case UnitType_FACTORY:
                goap->variables[HAS_AVAILABLE_FACTORY] = true;
                City* homeCity = (City*)Scene_GetComponent(scene, expansion->homeCity, CITY_COMPONENT_ID);
                if (Match_CityHasType(scene, homeCity, UnitType_AIRFIELD)) {
                    goap->variables[HAS_AVAILABLE_AIRFIELD] = true;
                }
                break;
            case UnitType_PORT:
                goap->variables[HAS_AVAILABLE_PORT] = true;
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
    if (Apricot_Keys[SDL_SCANCODE_LSHIFT]) {
        City* capitalCity = (City*)Scene_GetComponent(scene, nation->capital, CITY_COMPONENT_ID);
        printf("%s: ", capitalCity->name);
    }
}

// Really only for tactical planning, let GOAP handle strategic stuff, it's good at that
void AI_TargetGroundUnitsRandomly(Scene* scene)
{

    system(scene, id, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID, AI_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        if (unit->engaged) {
            continue;
        }

        // Setup patrol and dist
        bool isPatrol = Scene_EntityHasComponents(scene, id, PATROL_COMPONENT_ID);
        Patrol* patrol = NULL;
        if (isPatrol) {
            patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
        }
        float dist = isPatrol ? Vector_Dist(sprite->pos, patrol->patrolPoint) : Vector_Dist(sprite->pos, target->tar);
        if (dist > 1) {
            continue;
        }

        // Search through alerted tiles, go to closest one
        bool foundEnemy = false;
        Vector closestTile = { -1, -1 };
        float tempDist = FLT_MAX;
        for (int i = 0; i < nation->highPrioritySpaces->size; i++) {
            Vector point = *(Vector*)Arraylist_Get(nation->highPrioritySpaces, i);
            Vector scaledUp = Vector_Scalar(point, 32);
            float distance = Vector_Dist(sprite->pos, scaledUp);
            if (distance < tempDist && Terrain_LineOfSight(terrain, sprite->pos, scaledUp, sprite->z)) {
                tempDist = distance;
                closestTile = scaledUp;
                foundEnemy = true;
            }
        }
        if (foundEnemy) {
            if (isPatrol) {
                patrol->patrolPoint = closestTile;
            } else {
                target->tar = closestTile;
                target->lookat = closestTile;
            }
            continue;
        }

        // Go through units of the same nation
        float minDist = 2 * terrain->size;
        Vector closestOther = { -1, -1 };
        int engagedLevel = -1;
        system(scene, otherID, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID)
        {
            if (id == otherID)
                continue;
            Sprite* otherSprite = (Sprite*)Scene_GetComponent(scene, otherID, SPRITE_COMPONENT_ID);
            if (otherSprite->nation != nation) {
                continue;
            }
            Unit* otherUnit = (Unit*)Scene_GetComponent(scene, otherID, UNIT_COMPONENT_ID);
            if (otherUnit->engagedLevel || (otherUnit->engagedLevel >= unit->engagedLevel))
                continue;
            if (Vector_Dist(sprite->pos, otherSprite->pos) > minDist)
                continue;

            if (!Terrain_LineOfSight(terrain, sprite->pos, otherSprite->pos, sprite->z))
                continue;
            minDist = Vector_Dist(sprite->pos, otherSprite->pos);
            closestOther = otherSprite->pos;
            engagedLevel = otherUnit->engaged;
        }

        float randX = (float)(rand()) / (float)RAND_MAX - 0.5f;
        float randY = (float)(rand()) / (float)RAND_MAX - 0.5f;
        if (closestOther.x != -1) {
            unit->engagedLevel = 1 + engagedLevel;
            if (isPatrol) {
                patrol->patrolPoint = closestOther;
            } else {
                target->tar = Vector_Add(closestOther, Vector_Scalar(Vector_Normalize((Vector) { randX, randY }), 64));
                target->lookat = target->tar;
            }
        } else { // An alerted space could not be found, set unit's target randomly
            unit->engagedLevel = 100;
            Vector newTarget = Vector_Add(sprite->pos, Vector_Scalar(Vector_Normalize((Vector) { randX, randY }), 64));
            if (Terrain_LineOfSight(terrain, sprite->pos, newTarget, sprite->z)) {
                if (isPatrol) {
                    patrol->patrolPoint = newTarget;
                } else {
                    target->tar = newTarget;
                    target->lookat = newTarget;
                }
            }
        }
    }

    system(scene, id, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID, AI_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        Nation* nation = sprite->nation;
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        if (unit->engaged) {
            continue;
        }

        // Setup patrol and dist
        bool isPatrol = Scene_EntityHasComponents(scene, id, PATROL_COMPONENT_ID);
        Patrol* patrol = NULL;
        if (isPatrol) {
            patrol = (Patrol*)Scene_GetComponent(scene, id, PATROL_COMPONENT_ID);
        }
        float dist = isPatrol ? Vector_Dist(sprite->pos, patrol->patrolPoint) : Vector_Dist(sprite->pos, target->tar);
        if (dist > 1) {
            continue;
        }

        bool foundEnemy = false;
        Vector closestTile = { -1, -1 };
        float tempDist = FLT_MAX;

        // Spiral search for spaces to scout for unvisited spaces
        // SKIP if found alerted tile above
        int xOffsets[] = { 1, 0, -1, 0 };
        int yOffsets[] = { 0, 1, 0, -1 };
        int x = 0;
        int y = 0;
        int r = rand() % 4;
        int dx = xOffsets[r];
        int dy = yOffsets[r];
        for (int i = 0; !foundEnemy && i < nation->visitedSpacesSize * nation->visitedSpacesSize; i++) {
            Vector point = { x + (int)(sprite->pos.x / 32), y + (int)(sprite->pos.y / 32) };
            if (point.x >= 0 && point.y >= 0 && point.x < nation->visitedSpacesSize && point.y < nation->visitedSpacesSize) {
                float spaceValue = nation->visitedSpaces[(int)point.x + (int)(point.y) * nation->visitedSpacesSize];
                // If there is an enemy
                if (spaceValue <= 0) {
                    Vector newPoint = Vector_Scalar(point, 32);
                    float newDist = Vector_Dist(sprite->pos, newPoint);
                    // If near enough to target, or if new target is closer
                    if ((dist < 3 || newDist < dist) && Terrain_LineOfSight(terrain, sprite->pos, newPoint, sprite->z)) {
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
        }
    }
}

void AI_TargetEnemyCapital(Scene* scene, Nation* nation)
{
    system(scene, id, SPRITE_COMPONENT_ID, TARGET_COMPONENT_ID, UNIT_COMPONENT_ID, COMBATANT_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        if (sprite->nation != nation) {
            continue;
        }
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);

        for (int i = 0; i < nation->enemyNations->size; i++) {
            Nation* enemyNation = *(Nation**)Arraylist_Get(nation->enemyNations, i);
            if (enemyNation->capital == INVALID_ENTITY_INDEX) {
                continue;
            }
            Sprite* enemyCapital = (Sprite*)Scene_GetComponent(scene, enemyNation->capital, SPRITE_COMPONENT_ID);

            if (Terrain_LineOfSight(terrain, sprite->pos, enemyCapital->pos, sprite->z)) {
                target->tar = enemyCapital->pos;
                target->lookat = enemyCapital->pos;
                break;
            }
        }
    }
}

void AI_OrderInfantry(Scene* scene, Nation* nation)
{
    orderFromProducer(scene, nation, UnitType_ACADEMY, UnitType_INFANTRY);
}

void AI_OrderCavalry(Scene* scene, Nation* nation)
{
    if (rand() % 2 == 0) {
        orderFromProducer(scene, nation, UnitType_FACTORY, UnitType_CAVALRY);
    } else {
        orderFromProducer(scene, nation, UnitType_FACTORY, UnitType_ARTILLERY);
    }
}

void AI_OrderDestroyer(Scene* scene, Nation* nation)
{
    orderFromProducer(scene, nation, UnitType_PORT, UnitType_DESTROYER);
}

void AI_OrderFighter(Scene* scene, Nation* nation)
{
    orderFromProducer(scene, nation, UnitType_FACTORY, UnitType_FIGHTER);
}

void AI_OrderAttacker(Scene* scene, Nation* nation)
{
    orderFromProducer(scene, nation, UnitType_FACTORY, UnitType_ATTACKER);
}

void AI_OrderEngineer(Scene* scene, Nation* nation)
{
    orderFromProducer(scene, nation, UnitType_ACADEMY, UnitType_ENGINEER);
}

void AI_BuildCity(Scene* scene, Nation* nation)
{
    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        if (sprite->nation != nation) {
            continue;
        }
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);
        float dist = Vector_Dist(sprite->pos, target->tar);
        if (dist > 1) {
            continue;
        }

        float tempDistance = FLT_MAX;
        Vector tempTarget = { -1, -1 };
        for (int y = 0; y < terrain->tileSize; y++) {
            for (int x = 0; x < terrain->tileSize; x++) {
                if (Terrain_GetBuildingAt(terrain, x * 64 + 32, y * 64 + 32) != INVALID_ENTITY_INDEX)
                    continue;
                Vector point = { x * 64.0f + 32.0f, y * 64.0f + 32.0f };
                // Find if there is a city with cabdist less than 3 tiles
                // CAUSES ERRORS? First iteration too
                if (Terrain_ClosestMaskDist(scene, CITY_COMPONENT_ID, terrain, point.x, point.y) <= 2) {
                    continue;
                }
                if (Terrain_GetHeight(terrain, (int)point.x, (int)point.y) < 0.5)
                    continue;
                float distance = Vector_Dist(sprite->pos, point) + Terrain_GetHeight(terrain, (int)point.x, (int)point.y) * 100 - Terrain_GetOre(terrain, (int)point.x, (int)point.y) * 10;
                if (distance > tempDistance)
                    continue;
                if (!Terrain_LineOfSight(terrain, sprite->pos, point, 0.5)) {
                    continue;
                }
                tempTarget = point;
                tempDistance = distance;
            }
        }

        // City point found, set target
        if (tempTarget.x != -1) {
            target->tar = tempTarget;
            target->lookat = tempTarget;
            if (Vector_Dist(sprite->pos, target->tar) < 32) {
                Match_BuyCity(scene, sprite->nation, sprite->pos);
            }
        }
        break;
    }
}

void AI_BuildPortCity(Scene* scene, Nation* nation)
{
    system(scene, id, ENGINEER_UNIT_FLAG_COMPONENT_ID)
    {
        Sprite* sprite = (Sprite*)Scene_GetComponent(scene, id, SPRITE_COMPONENT_ID);
        if (sprite->nation != nation) {
            continue;
        }
        Target* target = (Target*)Scene_GetComponent(scene, id, TARGET_COMPONENT_ID);
        Unit* unit = (Unit*)Scene_GetComponent(scene, id, UNIT_COMPONENT_ID);

        float tempDistance = FLT_MAX;
        Vector tempTarget = { -1, -1 };
        for (int i = 0; i < terrain->ports->size; i++) {
            Vector portVector = *(Vector*)Arraylist_Get(terrain->ports, i);
            for (int y = -64; y <= 64; y += 64) {
                for (int x = -64; x <= 64; x += 64) {
                    if (abs(x) == abs(y)) {
                        continue;
                    }

                    Vector point = Vector_Add(portVector, (Vector) { x, y });
                    if (Terrain_GetBuildingAt(terrain, point.x, point.y) != INVALID_ENTITY_INDEX)
                        continue;
                    if (Terrain_ClosestMaskDist(scene, CITY_COMPONENT_ID, terrain, point.x, point.y) <= 2) {
                        continue;
                    }
                    if (Terrain_GetHeight(terrain, (int)point.x, (int)point.y) < 0.5)
                        continue;
                    float distance = Vector_Dist(sprite->pos, point) - Terrain_GetHeight(terrain, (int)point.x, (int)point.y) * 10;
                    if (distance > tempDistance)
                        continue;
                    if (!Terrain_LineOfSight(terrain, sprite->pos, point, 0.5))
                        continue;
                    Vector intersection = Terrain_LineOfSightPoint(terrain, sprite->pos, point, 0.5);
                    if (((int)(intersection.x / 64) + (int)(intersection.y / 64) * terrain->tileSize) == ((int)(sprite->pos.x / 64) + (int)(sprite->pos.y / 64) * terrain->tileSize)) {
                        continue;
                    }
                    tempTarget = point;
                    tempDistance = distance;
                }
            }
        }
        // City point found, set target
        if (tempTarget.x != -1) {
            target->tar = tempTarget;
            target->lookat = tempTarget;
            if (Vector_Dist(sprite->pos, target->tar) < 32) {
                Match_BuyCity(scene, sprite->nation, sprite->pos);
            }
        }
        break;
    }
}

void AI_BuildTimberland(Scene* scene, Nation* nation)
{
    findBuildingTile(scene, nation, UnitType_TIMBERLAND);
}

void AI_BuildMine(Scene* scene, Nation* nation)
{
    findBuildingTile(scene, nation, UnitType_MINE);
}

void AI_BuildFactory(Scene* scene, Nation* nation)
{
    findExpansionSpot(scene, nation, UnitType_FACTORY, false);
}

void AI_BuildFactoryForAirfield(Scene* scene, Nation* nation)
{
    findExpansionSpot(scene, nation, UnitType_FACTORY, true);
}

void AI_BuildPort(Scene* scene, Nation* nation)
{
    findPortTile(scene, nation);
}

void AI_BuildAirfield(Scene* scene, Nation* nation)
{
    findExpansionSpot(scene, nation, UnitType_AIRFIELD, false);
}

void AI_BuildFarm(Scene* scene, Nation* nation)
{
    findExpansionSpot(scene, nation, UnitType_FARM, false);
}

void AI_BuildAcademy(Scene* scene, Nation* nation)
{
    findExpansionSpot(scene, nation, UnitType_ACADEMY, false);
}

void AI_Init(Goap* goap)
{
    goap->updateVariableSystem = &AI_UpdateVariables;
    Goap_AddAction(goap, "Win", NULL, HAS_WON, 2, COMBATANTS_AT_ENEMY_CAPITAL, HAS_FIGHTER, 1, 4);

    Goap_AddAction(goap, "Target capital a", &AI_TargetEnemyCapital, COMBATANTS_AT_ENEMY_CAPITAL, 1, HAS_INFANTRY, 4);
    Goap_AddAction(goap, "Target capital b", &AI_TargetEnemyCapital, COMBATANTS_AT_ENEMY_CAPITAL, 1, HAS_CAVALRY, 3);
    Goap_AddAction(goap, "Target capital c", &AI_TargetEnemyCapital, COMBATANTS_AT_ENEMY_CAPITAL, 1, HAS_ATTACKER, 2);
    Goap_AddAction(goap, "Target capital d", &AI_TargetEnemyCapital, COMBATANTS_AT_ENEMY_CAPITAL, 1, SEA_SUPREMACY, 1);

    // Order ground units
    Goap_AddAction(goap, "Order infantry", &AI_OrderInfantry, HAS_INFANTRY, 4, HAS_ENGINEER, AFFORD_INFANTRY_COINS, HAS_AVAILABLE_ACADEMY, HAS_FOOD, 1, 1, 1, 1);
    Goap_AddAction(goap, "Order cavalry", &AI_OrderCavalry, HAS_CAVALRY, 4, AFFORD_CAVALRY_COINS, AFFORD_CAVALRY_ORE, HAS_AVAILABLE_FACTORY, HAS_FOOD, 1, 1, 1, 1);
    Goap_AddAction(goap, "Order engineer", &AI_OrderEngineer, HAS_ENGINEER, 3, AFFORD_ENGINEER_COINS, HAS_AVAILABLE_ACADEMY, HAS_FOOD, 1, 1, 1);

    // Order ships
    Goap_AddAction(goap, "Order destroyer", &AI_OrderDestroyer, SEA_SUPREMACY, 4, HAS_AVAILABLE_PORT, AFFORD_DESTROYER_COINS, AFFORD_DESTROYER_ORE, HAS_FOOD, 1, 1, 1, 1);

    // Order planes
    Goap_AddAction(goap, "Order fighter", &AI_OrderFighter, HAS_FIGHTER, 4, AFFORD_FIGHTER_COINS, AFFORD_FIGHTER_ORE, HAS_AVAILABLE_AIRFIELD, HAS_FOOD, 1, 1, 1, 1);
    Goap_AddAction(goap, "Order attacker", &AI_OrderAttacker, HAS_ATTACKER, 5, AFFORD_ATTACKER_COINS, AFFORD_ATTACKER_ORE, HAS_FIGHTER, HAS_AVAILABLE_AIRFIELD, HAS_FOOD, 1, 1, 1, 1, 1);

    // Build cities
    Goap_AddAction(goap, "city 4 coins", &AI_BuildCity, HAS_COINS, 5,
        AFFORD_CITY_COINS,
        AFFORD_CITY_TIMBER,
        ENGINEER_ISNT_BUSY,
        HAS_ENGINEER,
        HAS_TIMBER,
        1, 1, 1, 1, 1);
    Goap_AddAction(goap, "city 4 expa tile", &AI_BuildCity, SPACE_FOR_EXPANSION, 5,
        HAS_ENGINEER,
        AFFORD_CITY_COINS,
        AFFORD_CITY_TIMBER,
        ENGINEER_ISNT_BUSY,
        HAS_TIMBER,
        1, 1, 1, 1, 1);
    Goap_AddAction(goap, "city 4 2expa tile", &AI_BuildCity, SPACE_FOR_TWO_EXPANSIONS, 5,
        HAS_ENGINEER,
        AFFORD_CITY_COINS,
        AFFORD_CITY_TIMBER,
        ENGINEER_ISNT_BUSY,
        HAS_TIMBER,
        1, 1, 1, 1, 1);
    Goap_AddAction(goap, "city 4 port tile", &AI_BuildPortCity, SPACE_FOR_PORT, 7,
        HAS_ENGINEER,
        HAS_PORT_TILES,
        ENGINEER_CAN_SEE_PORT_CITY_TILE,
        AFFORD_CITY_COINS,
        AFFORD_CITY_TIMBER,
        ENGINEER_ISNT_BUSY,
        HAS_TIMBER,
        1, 1, 1, 1, 1, 1, 1);

    // Build timberlands
    Goap_AddAction(goap, "build timb", &AI_BuildTimberland, HAS_TIMBER, 3,
        HAS_ENGINEER,
        ENGINEER_ISNT_BUSY,
        AFFORD_TIMBERLAND_COINS,
        1, 1, 1);

    // Build farm
    Goap_AddAction(goap, "farm 4 food", &AI_BuildFarm, HAS_FOOD, 6,
        HAS_ENGINEER,
        ENGINEER_ISNT_BUSY,
        SPACE_FOR_EXPANSION,
        AFFORD_FARM_COINS,
        AFFORD_FARM_TIMBER,
        HAS_TIMBER,
        1, 1, 1, 1, 1, 1);

    // Build mine
    Goap_AddAction(goap, "mine 4 ore", &AI_BuildMine, HAS_ORE, 7,
        HAS_ENGINEER,
        ENGINEER_ISNT_BUSY,
        SPACE_FOR_EXPANSION,
        AFFORD_MINE_COINS,
        AFFORD_MINE_TIMBER,
        HAS_FOOD,
        HAS_TIMBER,
        1, 1, 1, 1, 1, 1, 1);

    // Build academy
    Goap_AddAction(goap, "acad", &AI_BuildAcademy, HAS_AVAILABLE_ACADEMY, 8,
        HAS_ENGINEER,
        ENGINEER_ISNT_BUSY,
        SPACE_FOR_EXPANSION,
        AFFORD_ACADEMY_COINS,
        AFFORD_ACADEMY_TIMBER,
        HAS_COINS,
        HAS_FOOD,
        HAS_TIMBER,
        1, 1, 1, 1, 1, 1, 1, 1);

    // Build factory
    Goap_AddAction(goap, "fact", &AI_BuildFactory, HAS_AVAILABLE_FACTORY, 9,
        HAS_ENGINEER,
        ENGINEER_ISNT_BUSY,
        SPACE_FOR_EXPANSION,
        AFFORD_FACTORY_COINS,
        AFFORD_FACTORY_TIMBER,
        HAS_COINS,
        HAS_ORE,
        HAS_FOOD,
        HAS_TIMBER,
        1, 1, 1, 1, 1, 1, 1, 1, 1);
    Goap_AddAction(goap, "fact 4 airf", &AI_BuildFactoryForAirfield, HAS_AVAILABLE_AIRFIELD, 9,
        HAS_ENGINEER,
        ENGINEER_ISNT_BUSY,
        SPACE_FOR_TWO_EXPANSIONS,
        AFFORD_FACTORY_COINS,
        AFFORD_FACTORY_TIMBER,
        HAS_COINS,
        HAS_ORE,
        HAS_FOOD,
        HAS_TIMBER,
        1, 1, 1, 1, 1, 1, 1, 1, 1);

    // Build port
    Goap_AddAction(goap, "port", &AI_BuildPort, HAS_AVAILABLE_PORT, 10,
        HAS_ENGINEER,
        ENGINEER_ISNT_BUSY,
        HAS_PORT_TILES,
        SPACE_FOR_PORT,
        AFFORD_PORT_COINS,
        AFFORD_PORT_TIMBER,
        HAS_COINS,
        HAS_ORE,
        HAS_FOOD,
        HAS_TIMBER,
        1, 1, 1, 1, 1, 1, 2, 1, 1, 1);

    // Build airfield
    Goap_AddAction(goap, "airf", &AI_BuildAirfield, HAS_AVAILABLE_AIRFIELD, 9,
        HAS_ENGINEER,
        ENGINEER_ISNT_BUSY,
        SPACE_FOR_EXPANSION,
        SPACE_FOR_AIRFIELD,
        AFFORD_AIRFIELD_COINS,
        AFFORD_AIRFIELD_TIMBER,
        HAS_AVAILABLE_FACTORY,
        HAS_FOOD,
        HAS_TIMBER,
        1, 1, 1, 1, 1, 1, 1, 1, 1);
}