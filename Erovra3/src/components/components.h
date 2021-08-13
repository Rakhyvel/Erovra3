#pragma once
#include <SDL.h>
#include <stdbool.h>

#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../util/vector.h"

void Components_Init(struct scene*);

/*
General guideline: Use flags for system iterations, and unit type for if statements
*/
typedef enum unitType {
    UnitType_INFANTRY,
    UnitType_CAVALRY,
    UnitType_ARTILLERY,
	UnitType_ENGINEER,
    UnitType_CITY,
    UnitType_MINE,
    UnitType_FACTORY,
    UnitType_PORT,
    UnitType_AIRFIELD,
    UnitType_WALL,
	UnitType_LANDING_CRAFT,
	UnitType_DESTROYER,
	UnitType_CRUISER,
	UnitType_BATTLESHIP,
	UnitType_AIRCRAFT_CARRIER,
    UnitType_FIGHTER,
    UnitType_ATTACKER,
    UnitType_BOMBER,
    _UnitType_Length
} UnitType;

typedef enum resourceType {
    ResourceType_COIN,
    ResourceType_ORE,
    ResourceType_POPULATION,
    ResourceType_POPULATION_CAPACITY,
    _ResourceType_Length
} ResourceType;

typedef enum cardinalDir {
	N,
	E,
	S,
	W
} CardinalDirection;

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
    float dZ; // Change in z (height)
    float aZ; // Change in dZ (vertical acceleration)
} Motion;
ComponentID MOTION_COMPONENT_ID;

/*
	Used by ground units and ships. These units go directly towards their target. */
typedef struct target {
    struct vector tar;
    struct vector lookat;
} Target;
ComponentID TARGET_COMPONENT_ID;

/*
	Used by air units only. Player specifies a patrol point, and unit moves 
	around this point */
typedef struct patrol {
    struct vector patrolPoint; // Where the patroller will be targeted to go by the user or by AI. Basically, the "general location" to go.
    struct vector focalPoint; // Where the patroller will actually go. Changed by systems that check for enemies. If no enemies, will likely be patrolPoint.
    float angle;
} Patrol;
ComponentID PATROL_COMPONENT_ID;

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
    ComponentMask sensedProjectiles;
} Health;
ComponentID HEALTH_COMPONENT_ID;

typedef struct unit {
    UnitType type;
    const float defense;
    int engagedTicks;
    char* name;
    bool stuckIn;
    bool engaged;
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
ComponentID ENGINEER_UNIT_FLAG_COMPONENT_ID;
ComponentID BUILDING_FLAG_COMPONENT_ID;
ComponentID WALL_FLAG_COMPONENT_ID;

ComponentID BULLET_ATTACK_FLAG_COMPONENT_ID;
ComponentID SHELL_ATTACK_FLAG_COMPONENT_ID;

ComponentID SHIP_FLAG_COMPONENT_ID;

ComponentID AIRCRAFT_FLAG_COMPONENT_ID;

// TODO: Add "origin" vector for projectiles, so that units can mark those areas as hostile
typedef struct projectile {
    const float attack;
    bool armed;
    const float splash;
} Projectile;
ComponentID PROJECTILE_COMPONENT_ID;

ComponentID AIR_BULLET_COMPONENT_ID;
ComponentID BULLET_COMPONENT_ID;
typedef struct shell {
    struct vector tar;
} Shell;
ComponentID SHELL_COMPONENT_ID;

ComponentID BOMB_COMPONENT_ID;

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
    int land;  // These are incremented when an order is fulfilled, and decremented when a unit dies 
    int sea;
    int air;
    int fighters;
    int airInProd;
    int fightersInProd;
} Nation;
ComponentID NATION_COMPONENT_ID;
ComponentID HOME_NATION_FLAG_COMPONENT_ID;
ComponentID ENEMY_NATION_FLAG_COMPONENT_ID;
ComponentID PLAYER_FLAG_COMPONENT_ID;
ComponentID AI_FLAG_COMPONENT_ID;

typedef struct city {
    char name[20];
    bool isCapital;
    EntityID expansions[4]; // Corresponds to NWSE CardinalDirection id
} City;
ComponentID CITY_COMPONENT_ID;

typedef struct producer {
    int orderTicksRemaining;
    UnitType order;
    bool repeat;
    const EntityID readyGUIContainer;
    const EntityID busyGUIContainer;
} Producer;
ComponentID PRODUCER_COMPONENT_ID;

typedef struct expansion {
    EntityID homeCity;
    CardinalDirection dir; // N=0 E=1 S=2 W=3
} Expansion;
ComponentID EXPANSION_COMPONENT_ID;

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