#pragma once
#include "nation.h"
#include "components.h"

/*
	Takes in a scene and color, registers a nation entity, assigns components 
	relevant to a nation. Return nation's EntityID */
EntityID Nation_Create(struct scene* scene, SDL_Color color, int mapSize, ComponentID ownNation, ComponentID enemyNation, ComponentID controlFlag)
{
    EntityID nationID = Scene_NewEntity(scene);
    Nation nation = {
        color,
        ownNation,
        enemyNation,
        controlFlag,
    };
	// Initial resources
    nation.resources[ResourceType_COIN] = 45;
    nation.resources[ResourceType_ORE] = 0;
    nation.resources[ResourceType_POPULATION] = 1;
    nation.resources[ResourceType_POPULATION_CAPACITY] = 2;

	// Coin costs
    nation.costs[ResourceType_COIN][UnitType_CITY] = 10;
    nation.costs[ResourceType_COIN][UnitType_FACTORY] = 10;
    nation.costs[ResourceType_COIN][UnitType_MINE] = 10;
    nation.costs[ResourceType_COIN][UnitType_PORT] = 10;
    nation.costs[ResourceType_COIN][UnitType_AIRFIELD] = 25;
    nation.costs[ResourceType_COIN][UnitType_INFANTRY] = 15;
    nation.costs[ResourceType_COIN][UnitType_CAVALRY] = 15;
    nation.costs[ResourceType_COIN][UnitType_ARTILLERY] = 15;
    nation.costs[ResourceType_COIN][UnitType_DESTROYER] = 15;
    nation.costs[ResourceType_COIN][UnitType_CRUISER] = 30;
    nation.costs[ResourceType_COIN][UnitType_BATTLESHIP] = 60;
    nation.costs[ResourceType_COIN][UnitType_FIGHTER] = 30;
    nation.costs[ResourceType_COIN][UnitType_ATTACKER] = 60;
    nation.costs[ResourceType_COIN][UnitType_BOMBER] = 120;

	// Ore costs
    nation.costs[ResourceType_ORE][UnitType_CITY] = 0;
    nation.costs[ResourceType_ORE][UnitType_FACTORY] = 0;
    nation.costs[ResourceType_ORE][UnitType_PORT] = 0;
    nation.costs[ResourceType_ORE][UnitType_MINE] = 0;
    nation.costs[ResourceType_ORE][UnitType_AIRFIELD] = 0;
    nation.costs[ResourceType_ORE][UnitType_CAVALRY] = 5;
    nation.costs[ResourceType_ORE][UnitType_ARTILLERY] = 5;
    nation.costs[ResourceType_ORE][UnitType_DESTROYER] = 5;
    nation.costs[ResourceType_ORE][UnitType_CRUISER] = 5;
    nation.costs[ResourceType_ORE][UnitType_BATTLESHIP] = 5;
    nation.costs[ResourceType_ORE][UnitType_FIGHTER] = 5;
    nation.costs[ResourceType_ORE][UnitType_ATTACKER] = 5;
    nation.costs[ResourceType_ORE][UnitType_BOMBER] = 5;

    nation.visitedSpacesSize = mapSize / 32;
    nation.visitedSpaces = malloc(nation.visitedSpacesSize * nation.visitedSpacesSize * sizeof(float));
    Scene_Assign(scene, nationID, controlFlag, NULL);
    Scene_Assign(scene, nationID, NATION_COMPONENT_ID, &nation);
    Scene_Assign(scene, nationID, ownNation, NULL);
    return nationID;
}

/*
	Sets the capital for a nation, and sets the visitedSpaces map according to the
	capital's position*/
void Nation_SetCapital(struct scene* scene, EntityID nationID, EntityID capital)
{
    Nation* nation = (Nation*)Scene_GetComponent(scene, nationID, NATION_COMPONENT_ID);
    Motion* capitalMotion = (Motion*)Scene_GetComponent(scene, capital, MOTION_COMPONENT_ID);
    nation->capital = capital;

    for (int y = 0; y < nation->visitedSpacesSize; y++) {
        for (int x = 0; x < nation->visitedSpacesSize; x++) {
            nation->visitedSpaces[x + y * nation->visitedSpacesSize] = (int)(2.56 * Vector_Dist(capitalMotion->pos, (Vector) { x * 32 + 16, y * 32 + 16 }) + 500);
        }
    }
}