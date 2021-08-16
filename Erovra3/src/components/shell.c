#pragma once
#include "../engine/textureManager.h"
#include "../textures.h"
#include "shell.h"
#include "../scenes/match.h"
#include "components.h"

/*
	Creates a shell entity, that starts at a given position, moves towards 
	another, has a given attack damage value, and belongs to a nation */
EntityID Shell_Create(struct scene* scene, Vector pos, Vector tar, float attack, EntityID nation)
{
    EntityID shellID = Scene_NewEntity(scene);

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
        0.8,
        true
    };
    Scene_Assign(scene, shellID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        SHELL_TEXTURE_ID,
        INVALID_TEXTURE_ID,
        INVALID_TEXTURE_ID,
		false,
        false,
        nation,
        4,
        4,
        0,
        0
    };
    Scene_Assign(scene, shellID, AIR_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, shellID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Shell shell = {
        tar
    };
    Scene_Assign(scene, shellID, SHELL_COMPONENT_ID, &shell);

    Projectile projectile = {
        attack,
		false,
		8.0f
    };
    Scene_Assign(scene, shellID, PROJECTILE_COMPONENT_ID, &projectile);

    Scene_Assign(scene, shellID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    Scene_Assign(scene, shellID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, controlFlag), NULL);
    return shellID;
}