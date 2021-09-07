#pragma once
#include "ai.h";

void AI_UpdateVariables(Scene* scene, Goap* goap, ComponentKey key)
{
}

void AI_Goal(Scene* scene, Goap* goap, ComponentKey key)
{
}

void AI_Init(Goap* goap)
{
    goap->updateVariableSystem = AI_UpdateVariables;
    Goap_AddAction(goap, AI_Goal, 0, HAS_WON, 3, NO_KNOWN_ENEMY_UNITS, FOUND_ENEMY_CAPITAL, COMBATANTS_AT_ENEMY_CAPITAL);
}