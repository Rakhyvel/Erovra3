#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

/* Creates a city entity with components assigned
* 
* @param scene	Pointer to the match scene
* @param pos	Position to put the city at
* @param nation	Nation EntityID for city
* @param nameBuffer	Null terminated string, less than 20 chars, that contains 
*					the city's name
* @param isCapital	Determines whether or not the city is a capital of a nation
* @returns EntityID of created city
*/
EntityID City_Create(struct scene*, Vector pos, EntityID nation, char* nameBuffer, bool isCapital);