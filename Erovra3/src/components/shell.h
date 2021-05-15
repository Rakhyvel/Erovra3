#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

EntityID Shell_Create(struct scene*, Vector pos, Vector tar, float attack, EntityID nation);
