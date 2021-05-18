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

typedef enum unitType {
	UnitType_INFANTRY,
	UnitType_CAVALRY,
    UnitType_ARTILLERY,
    UnitType_CITY,
    UnitType_MINE,
	UnitType_FACTORY,
	UnitType_WALL
} UnitType;

typedef struct unit {
    UnitType type;
    const float attack;
    const float defense;
    int randShoot;
    char* name;
    bool stuckIn;
    bool engaged;
} Unit;
ComponentID UNIT_COMPONENT_ID;
ComponentID LAND_UNIT_FLAG_COMPONENT_ID; // For buildings and ground units
ComponentID GROUND_UNIT_FLAG_COMPONENT_ID; // For infantry, cav, artill
ComponentID INFANTRY_UNIT_FLAG_COMPONENT_ID;
ComponentID BUILDING_FLAG_COMPONENT_ID;
ComponentID WALL_FLAG_COMPONENT_ID;

ComponentID BULLET_ATTACK_FLAG_COMPONENT_ID;
ComponentID SHELL_ATTACK_FLAG_COMPONENT_ID;

typedef struct projectile {
    const float attack;
    bool armed;
    const float splash;
} Projectile;
ComponentID PROJECTILE_COMPONENT_ID;

typedef struct shell {
    struct vector tar;
} Shell;
ComponentID SHELL_COMPONENT_ID;

typedef struct nation {
    SDL_Color color;
    ComponentID ownNationFlag;
    ComponentID enemyNationFlag;
    ComponentID controlFlag;
    int coins;
    int ore;
    int population;
    int popCapacity;
    int cityCost;
    int factoryCost;
    int mineCost;
    const int cavalryCost;
    const int artilleryCost;
    EntityID capital;
    EntityID enemyNation;
    float* visitedSpaces;
    int visitedSpacesSize;
} Nation;
ComponentID NATION_COMPONENT_ID;
ComponentID HOME_NATION_FLAG_COMPONENT_ID;
ComponentID ENEMY_NATION_FLAG_COMPONENT_ID;
ComponentID PLAYER_FLAG_COMPONENT_ID;
ComponentID AI_FLAG_COMPONENT_ID;

typedef struct city {
    char name[20];
    bool isCapital;
} City;
ComponentID CITY_COMPONENT_ID;

typedef struct producer {
    int orderTicksRemaining;
    UnitType order;
    bool repeat;
} Producer;
ComponentID PRODUCER_COMPONENT_ID;

ComponentID MINE_COMPONENT_ID;

ComponentID COIN_COMPONENT_ID;
ComponentID ORE_COMPONENT_ID;

typedef struct hoverable {
    bool isHovered;
} Hoverable;
ComponentID HOVERABLE_COMPONENT_ID;

typedef struct selectable {
    bool selected;
} Selectable;
ComponentID SELECTABLE_COMPONENT_ID;

typedef struct focusable {
    bool focused;
    EntityID guiContainer;
} Focusable;
ComponentID FOCUSABLE_COMPONENT_ID;