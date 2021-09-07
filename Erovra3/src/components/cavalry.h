#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

/* Creates a cavalry entity with components assigned
* 
* @param scene	Pointer to the match scene
* @param pos	Position to put the unit at
* @param nation	Nation EntityID for unit
* @returns EntityID of created cavalry
*/
EntityID Cavalry_Create(Scene* scene, Vector pos, EntityID nation);