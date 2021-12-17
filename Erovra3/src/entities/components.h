#pragma once
#include <SDL.h>
#include <stdbool.h>

#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../util/vector.h"
#include "../engine/goap.h"

struct nation;

/* Assigns random values to the global component IDs
*/
void Components_Init();

/* Registers each global component ID with the given scene
* 
* @param scene	Pointer to the match scene
*/
void Components_Register(struct scene*);

/* General guideline: Use flags for system iterations, and unit type for if statements
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
    UnitType_FARM,
    UnitType_ACADEMY,
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

/* In-game resources that a nation can have
*/
typedef enum resourceType {
    ResourceType_COIN,
    ResourceType_ORE,
    ResourceType_POPULATION,
    ResourceType_POPULATION_CAPACITY,
	ResourceType_FOOD,
    _ResourceType_Length
} ResourceType;

typedef enum cardinalDir {
    N, // North
    E, // East
    S, // South
    W // West
} CardinalDirection;

/* Layers are rendered one layer after another, to give a consistent depth
*/
enum RenderPriority {
    RenderPriorirty_BUILDING_LAYER, // Things on the ground (buildings, bullets)
    RenderPriorirty_SURFACE_LAYER, // Things on the surface (ground units, sea units)
    RenderPriority_HIGH_SURFACE_LAYER, // Things in between air and suface layer (bombs, shells)
    RenderPriorirty_AIR_LAYER, // Air units
    RenderPriorirty_PARTICLE_LAYER, // Coin particles, ore particles
};

/* Contains basic data for positioning an entity in the world space, as
   well as moving the entity with a target and velocity */
typedef struct sprite {
    struct vector pos; // The cartesian position of the unit in world-space
    float z; // The z axis coordinate. Ranges from [0,1], 0 being sea, 0.5 being surface, and 0.5-1 being air
    struct vector vel; // Difference in position over ticks
    float angle; // Angle the entity is oriented towads
    float speed; // Nominal speed of unit. Remains constant, while the velocity may change
    bool destroyOnBounds; // Whether or not the entity is purged if it goes outside the bounds of the map
    float dZ; // Change in z (height)
    float aZ; // Change in dZ (vertical acceleration)
    SDL_Texture* sprite; // Texture to draw on screen
    SDL_Texture* spriteOutline; // Texture to draw when an outline around sprite is shown
    SDL_Texture* shadow; // Texture to draw under sprite as a shadow
    enum RenderPriority priority; // Render layer that the sprite is on, like in paint.net or photoshop
    bool hidden; // Whether or not the sprite is hidden, and should not be rendered
    bool showOutline; // Whether or not the sprite should have an outline
    struct nation* nation; // Nation EntityID to get color from
    int width; // Width of the sprite when drawing to screen (asset may vary)
    int height; // Height of the sprite when drawing to screen (asset may vary)
    int outlineWidth; // Width of sprite outline texture
    int outlineHeight; // Height of sprite outline texture
    int hitTicks; // Timer, set when sprite is hit, systems fade this out. Used to add opacity to outline
} Sprite;
ComponentKey SPRITE_COMPONENT_ID;

/*	Used by ground units and ships. These units go directly towards their target. 

	Planes also are assigned this component, though their tar field is updated by
	systems based on their Patrol component*/
typedef struct target {
    struct vector tar; // Where to move towards
    struct vector lookat; // Where to face
    bool selected; // Whether or not the unit is selected
} Target;
ComponentKey TARGET_COMPONENT_ID;

/* Used by air units only. Player specifies a patrol point, and unit moves
* around this point */
typedef struct patrol {
    struct vector patrolPoint; // Where the patroller will be targeted to go by the user or by AI. Basically, the "general location" to go.
    struct vector focalPoint; // Where the patroller will actually go. Changed by systems that check for enemies. If no enemies, will likely be patrolPoint.
    float angle; // Acts like bacteria, this angle is updated to change focal point
} Patrol;
ComponentKey PATROL_COMPONENT_ID;

ComponentKey BUILDING_LAYER_COMPONENT_ID;
ComponentKey SURFACE_LAYER_COMPONENT_ID;
ComponentKey AIR_LAYER_COMPONENT_ID;
ComponentKey PLANE_LAYER_COMPONENT_ID;
ComponentKey PARTICLE_LAYER_COMPONENT_ID;

/* Misc. data relating to units
*/
typedef struct unit {
    float health; // Health of the unit. Full health is 100, dead is 0
    int aliveTicks; // How long this unit has been alive
    int deathTicks; // How long this unit has been dead, ranges from 0-16
    ComponentMask sensedProjectiles; // Mask of projectiles that this unit is vulnerable to
    bool isDead; // Whether or not this unit is dead
    UnitType type; // The type of unit this is
    const float defense; // The defense stat of the unit
    int ordinal; // Serial number of unit, assigned sequentially
    int engagedTicks; // Timer, ranges from 0-1/(unitspeed). Only determines if other nation can see unit
    bool isHovered; // Whether or not the unit is hovered
    bool focused; // Whether or not the unit is focused
    EntityID guiContainer; // The GUI container to show when focused
    char name[32]; // Description of unit
    bool engaged; // Whether the unit is engaged and cannot receive orders
    bool knownByEnemy; // Whether or not this unit has once been shown to the enemy
    bool foundAlertedSquare; // Whether or not this unit has found an alerted square and is investigating
} Unit;
ComponentKey UNIT_COMPONENT_ID;

/* Units that create projectiles and can attack other units
*/
typedef struct combatant {
    const float attack; // The attack stat of the unit, passed onto projectiles
    float attackDist; // How far the unit can engage the enemy, range of the unit
    ComponentMask enemyMask; // Components of other units that this unit attacks
    int attackTime; // Period of time for a projectile to be created
    void (*projConstructor)(struct scene*, Vector pos, Vector tar, float attack, struct nation* nation); // Function pointer to projectile constructor
    bool faceEnemy; // Whether or not the unit needs to face enemy (sea=no, else=yes)
    int randShoot; // A random timer to make shooting a bit random
} Combatant;
ComponentKey COMBATANT_COMPONENT_ID;

ComponentKey LAND_UNIT_FLAG_COMPONENT_ID; // For buildings and ground units
ComponentKey GROUND_UNIT_FLAG_COMPONENT_ID; // For infantry, cav, artill
ComponentKey ENGINEER_UNIT_FLAG_COMPONENT_ID;
ComponentKey BUILDING_FLAG_COMPONENT_ID;
ComponentKey WALL_FLAG_COMPONENT_ID;

ComponentKey BULLET_ATTACK_FLAG_COMPONENT_ID;
ComponentKey SHELL_ATTACK_FLAG_COMPONENT_ID;

ComponentKey SHIP_FLAG_COMPONENT_ID;

ComponentKey AIRCRAFT_FLAG_COMPONENT_ID;

// TODO: Add "origin" vector for projectiles, so that units can mark those areas as hostile
/* For projectile entities, which move and inflict damage on enemies
*/
typedef struct projectile {
    const float attack; // The attack damaged that the projectile does
    bool armed; // Whether or not the projectile does damage and should be sensed by units
    const float splash; // Splash radius of the projectile
} Projectile;
ComponentKey PROJECTILE_COMPONENT_ID;
ComponentKey AIR_BULLET_COMPONENT_ID;
ComponentKey BULLET_COMPONENT_ID;
ComponentKey SHELL_COMPONENT_ID;
ComponentKey BOMB_COMPONENT_ID;

/* Data used by nations
*/
typedef struct nation {
    SDL_Color color; // Color to render units
    ComponentKey controlFlag; // Flag that determines the control method for the nation
    int resources[_ResourceType_Length]; // Array of resource values
    int costs[_ResourceType_Length][_UnitType_Length]; // Array of array of resources, for each unit
    int unitCount[_UnitType_Length]; // How many units there are
    int prodCount[_UnitType_Length]; // How many units are in production currently
    EntityID capital; // EntityID of nation's capital
    Arraylist* enemyNations; // EntityIDs of enemy nations
    float* visitedSpaces; // Dynamic array, spaces that units have been to. Only used by AI's
    size_t visitedSpacesSize;  // Twice the tile size
    Arraylist* highPrioritySpaces; // A list of vectors of spaces where enemies have been spotted
    bool* showOre; // Dynamic array, whether or not to show ore at a tile
    Arraylist* cities; // List of cities, used by AI engineer to search for cities to build
    Goap goap; // A Goal-Oritented Action Planner for the AI
} Nation;

ComponentKey PLAYER_FLAG_COMPONENT_ID;

/* Assigned to nations that use a goal-oriented action planner to make decisions
*/
ComponentKey AI_COMPONENT_ID;

/* Data used by city entities
*/
typedef struct city {
    char name[20]; // Name of the city
    bool isCapital; // Whether or not the city is a capital
    EntityID expansions[4]; // Corresponds to NWSE CardinalDirection id
    Nation* captureNation; // The nation that this city will be captured by
} City;
ComponentKey CITY_COMPONENT_ID;

/* Data used by producers, which take resources and produce other units
*/
typedef struct producer {
    int orderTicksRemaining; // Ticks remaining before the unit is created
    UnitType order; // The order that the producer is making
    bool repeat; // Whether or not to repeat the order after it is completed
    const EntityID readyGUIContainer; // GUI container for when producer is not producing. Constant value to copy from for the focusable component
    const EntityID busyGUIContainer; // GUI container for when producer is producing. Constant value to copy from for the focusable component
    int orderTicksTotal; // Total number of ticks the order will take
} Producer;
ComponentKey PRODUCER_COMPONENT_ID;

/* Data for expansions, which are building units built adjacent to cities
*/
typedef struct expansion {
    EntityID homeCity; // The home city of the expansion
    CardinalDirection dir; // The direction the expansion is from the city. Used to index into city's expansion array
} Expansion;
ComponentKey EXPANSION_COMPONENT_ID;

/* Resource particles carry packets of resources from resource producers to the
* capital, where they are then converted into the nation's resources
*/
typedef struct resourceParticle {
    ResourceType type; // Resource type that this particle carries
    float distToCapital; // Initial distance from the resource producer to the capital
    Vector capitalPos; // Position of the capital at the time of creation
} ResourceParticle;
ComponentKey RESOURCE_PARTICLE_COMPONENT_ID;

/* Resource producers create resource particles, which then fly to the capital 
* to increment the nation's resources
*/
typedef struct resourceProducer {
    float produceRate; // The number of labor-ticks between each production
    void (*particleConstructor)(struct scene* scene, Vector pos, Nation* nation); // Resource particle constructor
} ResourceProducer;
ComponentKey RESOURCE_PRODUCER_COMPONENT_ID;

/* For order buttons, which order units to be built in producers
*/
typedef struct orderButton {
    SDL_Texture* icon; // Icon to display on the button
    UnitType type; // Type of unit to build, this retrieves the resource cost information for the button
} OrderButton;
ComponentKey ORDER_BUTTON_COMPONENT_ID;