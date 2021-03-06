#include "../engine/textureManager.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Shell_Create(struct scene* scene, Vector pos, Vector tar, float attack, Nation* nation)
{
    EntityID shellID = Scene_NewEntity(scene);

    Vector vel = Vector_Sub(tar, pos);
    float angle = Vector_Angle(vel);
    angle += 3.141592f / 2.0f;

    // Found from https://www.forrestthewoods.com/blog/solving_ballistic_trajectories/
    float s = 1.13f; // Projectile speed
    float g = 0.0005f; // Acceleration due to gravity
    float d = Vector_Dist(pos, tar);
    float root = powf(s, 4) - g * (g * powf(d, 2));
    root = sqrtf(root);
    float theta = atan((powf(s, 2) - root) / (g * d));
    vel = Vector_Normalize(vel);
    vel = Vector_Scalar(vel, s * cosf(theta));

    Sprite sprite = {
        SHELL_TEXTURE_ID,
        NULL,
        SHELL_SHADOW_TEXTURE_ID,
        nation,
        pos,
        vel,
        0.5f,
        angle,
        s * cosf(theta),
        s * sinf(theta),
        -g,
        RenderPriority_HIGH_SURFACE_LAYER,
        4,
        4,
        0,
        true,
        false,
        false,
    };
    Scene_Assign(scene, shellID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, shellID, AIR_LAYER_COMPONENT_ID, 0);

    Projectile projectile = {
        attack,
        false,
        8.0f
    };
    Scene_Assign(scene, shellID, PROJECTILE_COMPONENT_ID, &projectile);
    Scene_Assign(scene, shellID, SHELL_COMPONENT_ID, 0);

    Scene_Assign(scene, shellID, nation->controlFlag, NULL);
    return shellID;
}