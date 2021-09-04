#pragma once
#include "bomb.h"
#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "../util/arraylist.h"
#include "components.h"

/*
	Creates a bomb entity, that starts at a given position, moves towards 
	another, has a given attack damage value, and belongs to a nation */
EntityID Bomb_Create(struct scene* scene, Vector pos, Vector tar, float attack, EntityID nation)
{
    EntityID bombID = Scene_NewEntity(scene);

    float angle = Vector_Angle(Vector_Sub(pos, tar));
    angle += 3.1415f / 2.0f;
    Motion motion = {
        pos,
        1.0f,
        { 0.0f, 0.0f },
        angle,
        4,
        true,
        0.001, // Vel
        -0.0005f // Acc due to gravity
    };
    Scene_Assign(scene, bombID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        BOMB_TEXTURE_ID,
        INVALID_TEXTURE_ID,
        BOMB_SHADOW_TEXTURE_ID,
        RenderPriority_HIGH_SURFACE_LAYER,
        false,
        false,
        nation,
        9,
        5,
        0,
        0
    };
    Scene_Assign(scene, bombID, AIR_LAYER_COMPONENT_ID, 0);
    Scene_Assign(scene, bombID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);

    Projectile projectile = {
        attack,
        false,
        30.0f
    };
    Scene_Assign(scene, bombID, PROJECTILE_COMPONENT_ID, &projectile);
    Scene_Assign(scene, bombID, BOMB_COMPONENT_ID, NULL);

    Scene_Assign(scene, bombID, GET_COMPONENT_FIELD(scene, nation, NATION_COMPONENT_ID, Nation, ownNationFlag), NULL);
    return bombID;
}