#ifndef AI_H
#define AI_H

#include "../engine/scene.h"
#include "../engine/goap.h"

enum variables {
	HAS_WON,
	NO_KNOWN_ENEMY_UNITS,
	FOUND_ENEMY_CAPITAL,
	COMBATANTS_AT_ENEMY_CAPITAL
};

void AI_Init(Goap* goap);

#endif