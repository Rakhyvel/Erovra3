#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

/* Creates a coin entity with components assigned
* 
* @param scene	Pointer to the match scene
* @param pos	Position to put the coin at
* @param nation	Nation EntityID for coin
* @returns EntityID of created coin
*/
EntityID Coin_Create(struct scene*, Vector pos, EntityID nation);