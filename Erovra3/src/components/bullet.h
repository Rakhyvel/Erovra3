#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

/* Creates a bullet entity with components assigned
* 
* @param scene	Pointer to the match scene
* @param pos	Position to put the bullet at
* @param tar	Target position to fire towards
* @param attack	Attack damage dealt by bullet
* @param nation	Nation EntityID for bullet
* @returns EntityID of created bullet
*/
EntityID Bullet_Create(struct scene*, Vector pos, Vector tar, float attack, EntityID nation);

/* Creates an air bullet entity with components assigned
* 
* @param scene	Pointer to the match scene
* @param pos	Position to put the unit at
* @param tar	Position to fire towards
* @param attack Attack damage dealt by air bullet
* @param nation	Nation EntityID for unit
* @returns EntityID of created air bullet
*/
EntityID AirBullet_Create(struct scene* scene, Vector pos, Vector tar, float attack, EntityID nation);