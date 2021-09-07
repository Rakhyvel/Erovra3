#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

/* Creates an artillery entity with components assigned
* 
* @param scene	Pointer to the match scene
* @param pos	Position to put the artillery at
* @param nation	Nation EntityID for artillery unit
* @returns EntityID of created artillery unit
*/
EntityID Artillery_Create(Scene* scene, Vector pos, EntityID nation);
