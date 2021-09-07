#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

/* Creates an attacker entity with components assigned
* 
* @param scene	Pointer to the match scene
* @param pos	Position to put the attacker at
* @param nation	Nation EntityID for attacker
* @returns EntityID of created attacker
*/
EntityID Attacker_Create(Scene* scene, Vector pos, EntityID nation);