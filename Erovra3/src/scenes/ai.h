#ifndef AI_H
#define AI_H

#include "../engine/scene.h"
#include "../engine/goap.h"

enum variables {
	HAS_WON,

	// Win conditions
	NO_KNOWN_ENEMY_UNITS,
	FOUND_ENEMY_CAPITAL,
	COMBATANTS_AT_ENEMY_CAPITAL,

	// Engineer states
	ENGINEER_ISNT_BUSY,

	// "Has any # of unit" variables
	HAS_INFANTRY,
	HAS_CAVALRY,
	HAS_FIGHTER,
	HAS_ATTACKER,
	HAS_AVAILABLE_FACTORY, // Any factory that isn't producing an order
    HAS_AVAILABLE_AIRFIELD, // Any factory that isn't producing an order AND has an airfield
    HAS_AVAILABLE_ACADEMY, // Any academy that isn't producing an order
	HAS_COINS, // More like "has enough coin production"
	HAS_ORE, // More like "has enough ore production"
    HAS_POPULATION,

	SPACE_FOR_AIRFIELD,

	// "Can afford unit" variables
    AFFORD_INFANTRY_COINS,
    AFFORD_CAVALRY_COINS,
    AFFORD_CAVALRY_ORE,
    AFFORD_FIGHTER_COINS,
    AFFORD_FIGHTER_ORE,
    AFFORD_ATTACKER_COINS,
    AFFORD_ATTACKER_ORE,
    AFFORD_CITY_COINS,
    AFFORD_MINE_COINS,
    AFFORD_FACTORY_COINS,
    AFFORD_AIRFIELD_COINS,
	AFFORD_FARM_COINS,
	AFFORD_ACADEMY_COINS
};

void AI_Init(Goap* goap);

#endif