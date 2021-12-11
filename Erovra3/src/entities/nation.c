#include "./components.h"
#include "./entities.h"

EntityID Nation_Create(struct scene* scene, void (*goapInit)(Goap* goap), SDL_Color color, int mapSize, ComponentKey controlFlag)
{
    EntityID nationID = Scene_NewEntity(scene);
    Nation nation = {
        color,
        controlFlag
    };
    // Initial resources
    nation.resources[ResourceType_COIN] = 25;
    nation.resources[ResourceType_ORE] = 0;
    nation.resources[ResourceType_POPULATION] = 1;
    nation.resources[ResourceType_POPULATION_CAPACITY] = 1;
    nation.resources[ResourceType_FOOD] = 0;

    // Coin costs
    nation.costs[ResourceType_COIN][UnitType_CITY] = 10;
    nation.costs[ResourceType_COIN][UnitType_FACTORY] = 10;
    nation.costs[ResourceType_COIN][UnitType_MINE] = 10;
    nation.costs[ResourceType_COIN][UnitType_PORT] = 10;
    nation.costs[ResourceType_COIN][UnitType_AIRFIELD] = 20;
    nation.costs[ResourceType_COIN][UnitType_FARM] = 10;
    nation.costs[ResourceType_COIN][UnitType_ACADEMY] = 10;
    nation.costs[ResourceType_COIN][UnitType_WALL] = 10;
    nation.costs[ResourceType_COIN][UnitType_INFANTRY] = 15;
    nation.costs[ResourceType_COIN][UnitType_ENGINEER] = 15;
    nation.costs[ResourceType_COIN][UnitType_CAVALRY] = 15;
    nation.costs[ResourceType_COIN][UnitType_ARTILLERY] = 15;
    nation.costs[ResourceType_COIN][UnitType_DESTROYER] = 15;
    nation.costs[ResourceType_COIN][UnitType_CRUISER] = 30;
    nation.costs[ResourceType_COIN][UnitType_BATTLESHIP] = 60;
    nation.costs[ResourceType_COIN][UnitType_FIGHTER] = 15;
    nation.costs[ResourceType_COIN][UnitType_ATTACKER] = 15;
    nation.costs[ResourceType_COIN][UnitType_BOMBER] = 400;

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

    nation.visitedSpacesSize = mapSize / 32 + 1;
    nation.visitedSpaces = malloc(nation.visitedSpacesSize * nation.visitedSpacesSize * sizeof(float));

    nation.highPrioritySpaces = Arraylist_Create(10, sizeof(Vector));

    nation.showOre = calloc(mapSize / 64 * mapSize / 64, sizeof(bool));

	nation.enemyNations = Arraylist_Create(5, sizeof(EntityID));
    nation.cities = Arraylist_Create(10, sizeof(EntityID));

    if (controlFlag == AI_COMPONENT_ID) {
        AI ai;
        Scene_Assign(scene, nationID, controlFlag, &ai);
        AI* derefAI = (AI*)Scene_GetComponent(scene, nationID, AI_COMPONENT_ID);
        Goap_Create(&(derefAI->goap), goapInit);
    } else {
        Scene_Assign(scene, nationID, controlFlag, NULL);
    }
    Scene_Assign(scene, nationID, NATION_COMPONENT_ID, &nation);
    return nationID;
}

void Nation_SetCapital(struct scene* scene, EntityID nationID, EntityID capital)
{
    Nation* nation = (Nation*)Scene_GetComponent(scene, nationID, NATION_COMPONENT_ID);
    Motion* capitalMotion = (Motion*)Scene_GetComponent(scene, capital, MOTION_COMPONENT_ID);
    nation->capital = capital;

    for (int y = 0; y < nation->visitedSpacesSize; y++) {
        for (int x = 0; x < nation->visitedSpacesSize; x++) {
            nation->visitedSpaces[x + y * nation->visitedSpacesSize] = (float)floor(5 * Vector_Dist(capitalMotion->pos, (Vector) { x * 32.0f + 16.0f, y * 32.0f + 16.0f }) + 1000);
        }
    }
}