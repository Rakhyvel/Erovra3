#pragma once
#include "../engine/textureManager.h"
#include "bullet.h"
#include "../textures.h"
#include "components.h"

EntityID Bullet_Create(struct scene* scene, Vector pos, Vector tar, float attack, EntityID nation)
{
    EntityID bulletID = Scene_NewEntity(scene);

    Vector vel = Vector_Sub(tar, pos);
    Vector_Normalize(vel);
    vel = Vector_Scalar(vel, 4);
    float angle = Vector_Angle(vel);
    angle += 3.141592f / 2.0f;
    Motion motion = {
        pos,
        0.5f,
        vel,
        angle,
        4,
        true
    };
    Scene_Assign(scene, bulletID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        BULLET_TEXTURE_ID,
		INVALID_TEXTURE_ID,
        INVALID_TEXTURE_ID,
        false,
        nation,
        20,
        2,
        0,
        0
    };
    Scene_Assign(scene, bulletID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

	Projectile projectile = {
        attack
    };
    Scene_Assign(scene, bulletID, PROJECTILE_COMPONENT_ID, &projectile);

    Scene_Assign(scene, bulletID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    return bulletID;
}