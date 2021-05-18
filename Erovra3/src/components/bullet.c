#pragma once
#include "../engine/textureManager.h"
#include "bullet.h"
#include "../textures.h"
#include "components.h"
#include "../util/arraylist.h"
#include "../engine/scene.h"

/*
	Creates a bullet entity, that starts at a given position, moves towards 
	another, has a given attack damage value, and belongs to a nation */
EntityID Bullet_Create(struct scene* scene, Vector pos, Vector tar, float attack, EntityID nation)
{
    EntityID bulletID = Scene_NewEntity(scene);

    Vector vel = Vector_Sub(tar, pos);
    vel = Vector_Normalize(vel);
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
        false,
        nation,
        20,
        2,
        0,
        0
    };
    Scene_Assign(scene, bulletID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

	Projectile projectile = {
        attack,
		true,
		8.0f
    };
    Scene_Assign(scene, bulletID, PROJECTILE_COMPONENT_ID, &projectile);

    Scene_Assign(scene, bulletID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    return bulletID;
}