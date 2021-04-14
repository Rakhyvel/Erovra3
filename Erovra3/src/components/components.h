#pragma once
#include <SDL.h>
#include <stdbool.h>

#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../util/vector.h"

void Components_Init(struct scene*);

/*
		Contains basic data for positioning an entity in the world space, as
   well as moving the entity with a target and velocity */
typedef struct motion {
    struct vector pos;
    float z;
    struct vector vel;
    float angle;
    float speed;
    bool destroyOnBounds;
} Motion;
ComponentID MOTION_COMPONENT_ID;

typedef struct target {
    struct vector tar;
    struct vector lookat;
} Target;
ComponentID TARGET_COMPONENT_ID;

/*
		Contains data for rendering an entity to the screen, like it's sprite,
   the outline of the sprite, and whether or not the outline should show */
typedef struct simpleRenderable {
    TextureID sprite;
    TextureID spriteOutline;
    TextureID shadow;
    bool showOutline;
    EntityID nation;
    int width;
    int height;
    int outlineWidth;
    int outlineHeight;
    int hitTicks;
} SimpleRenderable;
ComponentID SIMPLE_RENDERABLE_COMPONENT_ID;

/*
		Contains data used for determining the health of a unit, how long the
   unit has been alive, and how long the unit has been dead. */
typedef struct health {
    float health;
    int aliveTicks;
    int deathTicks;
} Health;
ComponentID HEALTH_COMPONENT_ID;

typedef struct unit {
    const float attack;
    const float defense;
    bool stuckIn;
    bool engaged;
} Unit;
ComponentID UNIT_COMPONENT_ID;
ComponentID LAND_UNIT_FLAG_COMPONENT_ID;
ComponentID GROUND_UNIT_FLAG_COMPONENT_ID;

typedef struct projectile {
    const float attack;
} Projectile;
ComponentID PROJECTILE_COMPONENT_ID;

typedef struct nation {
    SDL_Color color;
    ComponentID ownNationFlag;
    ComponentID enemyNationFlag;
    EntityID capital;
    EntityID enemyNation;
} Nation;
ComponentID NATION_COMPONENT_ID;
ComponentID HOME_NATION_FLAG_COMPONENT_ID;
ComponentID ENEMY_NATION_FLAG_COMPONENT_ID;

typedef struct city {
    char name[20];
    bool isCapital;
    float oreMined;
} City;
ComponentID CITY_COMPONENT_ID;

typedef struct selectable {
    bool isHovered;
    bool selected;
} Selectable;
ComponentID SELECTABLE_COMPONENT_ID;
