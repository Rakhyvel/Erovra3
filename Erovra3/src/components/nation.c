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
        45, // coins
        0, // ore
        1, // pop.
        5, // pop. cap
        5, // city cost
        10, // factory cost
        10, // mine cost
        15, // cavalry cost
        15 // artillery cost
    };
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