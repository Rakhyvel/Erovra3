#pragma once
#include <SDL.h>
#include <stdbool.h>
#include "vector.h"
#include "scene.h"

/*
	Contains basic data for positioning an entity in the world space, as well as 
	moving the entity with a target and velocity */
typedef struct transform {
	struct vector pos;
	float z;
	struct vector lookat;
	struct vector velocity;
	struct vector target;
	double angle;
} Transform;
const ComponentID transformID = 0;

/*
	Contains data for rendering an entity to the screen, like it's sprite, the 
	outline of the sprite, and whether or not the outline should show */
typedef struct simpleRenderable {
	SDL_Texture* sprite;
	SDL_Texture* spriteOutline;
	bool showOutline;
} SimpleRenderable;
const ComponentID simpleRenderableID = 1;

/*
	Contains data used for determining the health of a unit, how long the unit
	has been alive, and how long the unit has been dead. */
typedef struct health {
	float health;
	int aliveTicks;
	int deathTicks;
} Health;
const ComponentID healthID = 2;

typedef struct unitType {
	float attack;
	float speed;
	float defense;
} UnitType;
const ComponentID unitTypeID = 3;

typedef struct city {
	char name[20];
	bool isCapital;
	float oreMined;
} City;
const ComponentID cityID = 4;