#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

EntityID Port_Create(struct scene*, Vector pos, EntityID nation, EntityID homeCity, CardinalDirection dir);
