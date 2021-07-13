#pragma once
#include <SDL.h>
#include <stdbool.h>

#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../util/vector.h"

void Components_Init(struct scene*);

typedef enum unitType {
    UnitType_INFANTRY,
    UnitType_CAVALRY,
    UnitType_ARTILLERY,
    UnitType_CITY,
    UnitType_MINE,
    UnitType_FACTORY,
	UnitType_PORT,
    UnitType_WALL,
	UnitType_LANDING_CRAFT,
	UnitType_DESTROYER,
	UnitType_CRUISER,
	UnitType_BATTLESHIP,
	UnitType_AIRCRAFT_CARRIER,
    _UnitType_Length
} UnitType;

typedef enum resourceType {
    ResourceType_COIN,
    ResourceType_ORE,
    ResourceType_POPULATION,
    ResourceType_POPULATION_CAPACITY,
    _ResourceType_Length
} ResourceType;

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
    struct vector scoutLock;
} Target;
ComponentID TARGET_COMPONENT_ID;

/*
		Contains data for rendering an entity to the screen, like it's sprite,
   the outline of the sprite, and whether or not the outline should show */
typedef struct simpleRenderable {
    TextureID sprite;
    TextureID spriteOutline;
    TextureID shadow;
    bool hidden;
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
    UnitType type;
    const float defense;
    char* name;
    bool stuckIn;
    bool engaged;
    int engagedTicks;
} Unit;
ComponentID UNIT_COMPONENT_ID;

typedef struct combatant {
    const float attack;
    float attackDist;
    ComponentMask enemyMask;
    int attackTime;
    void (*projConstructor)(struct scene*, Vector pos, Vector tar, float attack, EntityID nation);
    bool faceEnemy;
    int randShoot;
} Combatant;
ComponentID COMBATANT_COMPONENT_ID;

ComponentID LAND_UNIT_FLAG_COMPONENT_ID; // For buildings and ground units
ComponentID GROUND_UNIT_FLAG_COMPONENT_ID; // For infantry, cav, artill
ComponentID INFANTRY_UNIT_FLAG_COMPONENT_ID;
ComponentID BUILDING_FLAG_COMPONENT_ID;
ComponentID WALL_FLAG_COMPONENT_ID;

ComponentID BULLET_ATTACK_FLAG_COMPONENT_ID;
ComponentID SHELL_ATTACK_FLAG_COMPONENT_ID;

ComponentID SHIP_FLAG_COMPONENT_ID;

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
    ComponentID ownNationFlag; // Flag that determines if an entity belongs to this nation
    ComponentID enemyNationFlag; // Flag that determines if an entity belongs to enemy nation
    ComponentID controlFlag;
    int resources[_ResourceType_Length];
    int costs[_ResourceType_Length][_UnitType_Length];
    EntityID capital;
    EntityID enemyNation; // EntityID of other nation, NOT the flag for enemy nation
    float* visitedSpaces;
    int visitedSpacesSize;
    int cities;
    int factories;
    int mines;
    int land;
    int sea;
} Nation;
ComponentID NATION_COMPONENT_ID;
ComponentID HOME_NATION_FLAG_COMPONENT_ID;
ComponentID ENEMY_NATION_FLAG_COMPONENT_ID;
ComponentID PLAYER_FLAG_COMPONENT_ID;
ComponentID AI_FLAG_COMPONENT_ID;

typedef struct city {
    char name[20];
    bool isCapital;
    EntityID buildings[4]; // Corresponds to NWSE cardinal directions
} City;
ComponentID CITY_COMPONENT_ID;

typedef struct producer {
    int orderTicksRemaining;
    UnitType order;
    bool repeat;
    EntityID homeCity;
    const EntityID readyGUIContainer;
    const EntityID busyGUIContainer;
} Producer;
ComponentID PRODUCER_COMPONENT_ID;

typedef struct resourceParticle {
    ResourceType type;
} ResourceParticle;
ComponentID RESOURCE_PARTICLE_COMPONENT_ID;

typedef struct resourceProducer {
    float produceRate;
    void (*particleConstructor)(struct scene* scene, Vector pos, EntityID nationID);
} ResourceProducer;
ComponentID RESOURCE_PRODUCER_COMPONENT_ID;

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