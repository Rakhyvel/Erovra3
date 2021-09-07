#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"
#include "./components.h"

/* Creates an academy entity with components assigned
* 
* @param scene	Pointer to the match scene
* @param pos	Position to put the academy at
* @param nation	Nation EntityID for academy
* @param homeCity	EntityID of home city
* @param dir	Offset direction relative to home city
* @returns EntityID of created academy
*/
EntityID Academy_Create(struct scene*, Vector pos, EntityID nation, EntityID homeCity, CardinalDirection dir);
