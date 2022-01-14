#include "./assemblages.h"
#include "./components.h"
#include <string.h>

void Nation_Create(struct scene* scene, Nation* nation, void (*goapInit)(Goap* goap), SDL_Color color, int mapSize, ComponentKey controlFlag)
{
    nation->capital = INVALID_ENTITY_INDEX;
    nation->color = color;
    nation->controlFlag = controlFlag;

    Nation_ResetResources(scene, nation);

    nation->visitedSpacesSize = mapSize / 32 + 1;
    nation->visitedSpaces = malloc(nation->visitedSpacesSize * nation->visitedSpacesSize * sizeof(float));

    nation->highPrioritySpaces = Arraylist_Create(10, sizeof(Vector));

    nation->enemyNations = Arraylist_Create(5, sizeof(Nation*));
    nation->cities = Arraylist_Create(5, sizeof(EntityID));

    if (controlFlag == AI_COMPONENT_ID) {
        Goap_Create(&(nation->goap), goapInit);
    }
}

void Nation_ResetResources(Scene* scene, Nation* nation)
{
	// Do not assume this has been done, especially for recycled nations
    memset(nation->resources, 0, sizeof(nation->resources));
    memset(nation->costs, 0, sizeof(nation->costs));

    // Initial resources
    nation->resources[ResourceType_POPULATION] = 1;
    nation->resources[ResourceType_COIN] = 25;
    nation->resources[ResourceType_TIMBER] = 4;

    // Coin costs
    nation->costs[ResourceType_COIN][UnitType_CITY] = 5;
    nation->costs[ResourceType_COIN][UnitType_TIMBERLAND] = 5;
    nation->costs[ResourceType_COIN][UnitType_FACTORY] = 10;
    nation->costs[ResourceType_COIN][UnitType_MINE] = 5;
    nation->costs[ResourceType_COIN][UnitType_POWERPLANT] = 10;
    nation->costs[ResourceType_COIN][UnitType_FOUNDRY] = 10;
    nation->costs[ResourceType_COIN][UnitType_PORT] = 10;
    nation->costs[ResourceType_COIN][UnitType_AIRFIELD] = 10;
    nation->costs[ResourceType_COIN][UnitType_FARM] = 5;
    nation->costs[ResourceType_COIN][UnitType_ACADEMY] = 10;
    nation->costs[ResourceType_COIN][UnitType_WALL] = 10;
    nation->costs[ResourceType_COIN][UnitType_INFANTRY] = 20;
    nation->costs[ResourceType_COIN][UnitType_ENGINEER] = 15;
    nation->costs[ResourceType_COIN][UnitType_CAVALRY] = 15;
    nation->costs[ResourceType_COIN][UnitType_ARTILLERY] = 15;
    nation->costs[ResourceType_COIN][UnitType_DESTROYER] = 15;
    nation->costs[ResourceType_COIN][UnitType_CRUISER] = 30;
    nation->costs[ResourceType_COIN][UnitType_BATTLESHIP] = 60;
    nation->costs[ResourceType_COIN][UnitType_FIGHTER] = 15;
    nation->costs[ResourceType_COIN][UnitType_ATTACKER] = 15;
    nation->costs[ResourceType_COIN][UnitType_BOMBER] = 20;

    // Timber costs
    nation->costs[ResourceType_TIMBER][UnitType_CITY] = 5;
    nation->costs[ResourceType_TIMBER][UnitType_FACTORY] = 5;
    nation->costs[ResourceType_TIMBER][UnitType_MINE] = 0;
    nation->costs[ResourceType_TIMBER][UnitType_POWERPLANT] = 5;
    nation->costs[ResourceType_TIMBER][UnitType_FOUNDRY] = 5;
    nation->costs[ResourceType_TIMBER][UnitType_PORT] = 5;
    nation->costs[ResourceType_TIMBER][UnitType_AIRFIELD] = 5;
    nation->costs[ResourceType_TIMBER][UnitType_ACADEMY] = 5;
    nation->costs[ResourceType_TIMBER][UnitType_WALL] = 5;

    // Metal costs
    nation->costs[ResourceType_METAL][UnitType_CAVALRY] = 5;
    nation->costs[ResourceType_METAL][UnitType_ARTILLERY] = 5;
    nation->costs[ResourceType_METAL][UnitType_DESTROYER] = 10;
    nation->costs[ResourceType_METAL][UnitType_CRUISER] = 10;
    nation->costs[ResourceType_METAL][UnitType_BATTLESHIP] = 10;
    nation->costs[ResourceType_METAL][UnitType_FIGHTER] = 10;
    nation->costs[ResourceType_METAL][UnitType_ATTACKER] = 15;
    nation->costs[ResourceType_METAL][UnitType_BOMBER] = 20;
}

void Nation_SetCapital(struct scene* scene, Nation* nation, EntityID capital)
{
    Sprite* capitalSprite = (Sprite*)Scene_GetComponent(scene, capital, SPRITE_COMPONENT_ID);
    nation->capital = capital;
    nation->capitalPos = capitalSprite->pos;

    ResourceAccepter accepter;
    memset(&accepter, 0, sizeof(accepter));
    accepter.capacity[ResourceType_COIN] = 1000000;
    accepter.capacity[ResourceType_TIMBER] = 1000000;
    accepter.capacity[ResourceType_METAL] = 1000000;
    Scene_Assign(scene, capital, RESOURCE_ACCEPTER_COMPONENT_ID, &accepter);

    for (int y = 0; y < nation->visitedSpacesSize; y++) {
        for (int x = 0; x < nation->visitedSpacesSize; x++) {
            float dist = Vector_Dist(capitalSprite->pos, (Vector) { x * 32.0f + 16.0f, y * 32.0f + 16.0f });
            nation->visitedSpaces[x + y * nation->visitedSpacesSize] = (float)floor(0.88 * dist + 1000);
        }
    }
}