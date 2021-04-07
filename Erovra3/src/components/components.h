#pragma once
#include <SDL.h>
#include <stdbool.h>
#include "../util/vector.h"
#include "../engine/scene.h"
#include "../engine/textureManager.h"

void Components_Init(struct scene*);

/*
	Contains basic data for positioning an entity in the world space, as well as 
	moving the entity with a target and velocity */
typedef struct transform {
	struct vector pos;
	float z;
	struct vector lookat;
	struct vector vel;
	struct vector tar;
	double angle;
} Transform;
ComponentID TRANSFORM_COMPONENT_ID;

/*
	Contains data for rendering an entity to the screen, like it's sprite, the 
	outline of the sprite, and whether or not the outline should show */
typedef struct simpleRenderable {
	TextureID sprite;
	TextureID spriteOutline;
	bool showOutline;
} SimpleRenderable;
ComponentID SIMPLE_RENDERABLE_COMPONENT_ID;

/*
	Contains data used for determining the health of a unit, how long the unit
	has been alive, and how long the unit has been dead. */
typedef struct health {
	float health;
	int aliveTicks;
	int deathTicks;
} Health;
ComponentID HEALTH_COMPONENT_ID;

typedef struct unitType {
	float attack;
	float speed;
	float defense;
} UnitType;
ComponentID UNIT_TYPE_COMPONENT_ID;

typedef struct city {
	char name[20];
	bool isCapital;
	float oreMined;
} City;
ComponentID CITY_COMPONENT_ID;