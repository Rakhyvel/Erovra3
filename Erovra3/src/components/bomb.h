#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

/* Creates a bomb entity with components assigned
* 
* @param scene	Pointer to the match scene
* @param pos	Position to put the bomb at
* @param tar	Position to move towards (not really used in this case)
* @param attack	The attack damage dealt by the bomb
* @param nation	Nation EntityID for bomb
* @returns EntityID of created bomb
*/
EntityID Bomb_Create(struct scene*, Vector pos, Vector tar, float attack, EntityID nation);