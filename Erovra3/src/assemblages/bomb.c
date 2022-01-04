#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "../util/arraylist.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Bomb_Create(struct scene* scene, Vector pos, Vector tar, float attack, Nation* nation)
{
    EntityID bombID = Scene_NewEntity(scene);

    float angle = Vector_Angle(Vector_Sub(pos, tar));
    angle += 3.1415f / 2.0f;
    Sprite sprite = {
        BOMB_TEXTURE_ID,
        NULL,
        BOMB_SHADOW_TEXTURE_ID,
        nation,
        pos,
        { 0.0f, 0.0f },
        1.0f,
        angle,
        4,
        0.001, // Vel
        -0.0005f, // Acc due to gravity
        RenderPriority_HIGH_SURFACE_LAYER,
        9,
        5,
        0,
        true,
        false,
        false,
    };
    Scene_Assign(scene, bombID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, bombID, AIR_LAYER_COMPONENT_ID, 0);

    Projectile projectile = {
        attack,
        false,
        30.0f
    };
    Scene_Assign(scene, bombID, PROJECTILE_COMPONENT_ID, &projectile);
    Scene_Assign(scene, bombID, BOMB_COMPONENT_ID, NULL);

    return bombID;
}