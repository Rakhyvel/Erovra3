#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"
#include "./components.h"

EntityID Farm_Create(struct scene*, Vector pos, EntityID nation, EntityID homeCity, CardinalDirection dir);