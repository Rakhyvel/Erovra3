#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

EntityID City_Create(struct scene*, Vector pos, EntityID nation, char* nameBuffer, bool isCapital);