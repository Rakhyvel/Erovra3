#pragma once
#include "../engine/scene.h"
#include "../util/vector.h"

EntityID Bullet_Create(struct scene*, Vector pos, Vector tar, float attack, EntityID nation);
EntityID AirBullet_Create(struct scene* scene, Vector pos, Vector tar, float attack, EntityID nation);