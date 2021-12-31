#ifndef AI_H
#define AI_H

#include "../engine/goap.h"
#include "../engine/scene.h"

enum variables {
    HAS_WON,

    COMBATANTS_AT_ENEMY_CAPITAL,

    ALWAYS_TRUE,

    // Win conditions
    NO_KNOWN_ENEMY_UNITS,
    FOUND_ENEMY_CAPITAL,
    SEA_SUPREMACY,

    // Engineer states
    ENGINEER_ISNT_BUSY,

    // "Has any # of unit" variables
    HAS_ENGINEER,
    HAS_INFANTRY,
    HAS_CAVALRY,
    HAS_FIGHTER,
    HAS_ATTACKER,
    HAS_AVAILABLE_FACTORY, // Any factory that isn't producing an order
	HAS_AVAILABLE_PORT, // Any port that isn't producing an order
    HAS_AVAILABLE_AIRFIELD, // Any factory that isn't producing an order AND has an airfield
    HAS_AVAILABLE_ACADEMY, // Any academy that isn't producing an order
    HAS_COINS, // More like "has enough coin production"
    HAS_ORE, // More like "has enough ore production"
    HAS_FOOD, // More like "has enough food production"

    SPACE_FOR_EXPANSION,
    SPACE_FOR_TWO_EXPANSIONS,
    SPACE_FOR_AIRFIELD,
	SPACE_FOR_PORT,
	HAS_PORT_TILES,
	HAS_NO_PORT_TILES,
    ENGINEER_CAN_SEE_PORT_CITY_TILE,

    // "Can afford unit" variables
    AFFORD_INFANTRY_COINS,
    AFFORD_CAVALRY_COINS,
    AFFORD_CAVALRY_ORE,
    AFFORD_DESTROYER_COINS,
    AFFORD_DESTROYER_ORE,
    AFFORD_FIGHTER_COINS,
    AFFORD_FIGHTER_ORE,
    AFFORD_ATTACKER_COINS,
    AFFORD_ATTACKER_ORE,
    AFFORD_ENGINEER_COINS,

    AFFORD_CITY_COINS,
	AFFORD_CITY_TIMBER,

	AFFORD_TIMBERLAND_COINS,

    AFFORD_FARM_COINS,
    AFFORD_FARM_TIMBER,

    AFFORD_MINE_COINS,
    AFFORD_MINE_TIMBER,

    AFFORD_ACADEMY_COINS,
    AFFORD_ACADEMY_TIMBER,

    AFFORD_FACTORY_COINS,
    AFFORD_FACTORY_TIMBER,

    AFFORD_PORT_COINS,
    AFFORD_PORT_TIMBER,

    AFFORD_AIRFIELD_COINS,
    AFFORD_AIRFIELD_TIMBER,
};

void AI_TargetGroundUnitsRandomly(Scene* scene);
void AI_Init(Goap* goap);

#endif