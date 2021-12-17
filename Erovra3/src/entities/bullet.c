#include "../engine/scene.h"
#include "../engine/textureManager.h"
#include "../scenes/match.h"
#include "../textures.h"
#include "../util/arraylist.h"
#include "./components.h"
#include "./entities.h"

EntityID Bullet_Create(struct scene* scene, Vector pos, Vector tar, float attack, Nation* nation)
{
    EntityID bulletID = Scene_NewEntity(scene);

    Vector vel = Vector_Sub(tar, pos);
    vel = Vector_Normalize(vel);
    vel = Vector_Scalar(vel, 4);
    float angle = Vector_Angle(vel);
    angle += 3.141592f / 2.0f;
    Sprite sprite = {
        pos,
        0.5f,
        vel,
        angle,
        4,
        true,
		0,
        0,
        BULLET_TEXTURE_ID,
        NULL,
        BULLET_SHADOW_TEXTURE_ID,
        RenderPriorirty_BUILDING_LAYER,
        false,
        false,
        nation,
        20,
        2,
        0,
        0
    };
    Scene_Assign(scene, bulletID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, bulletID, BUILDING_LAYER_COMPONENT_ID, 0);

    Projectile projectile = {
        attack,
        true,
        12.0f
    };
    Scene_Assign(scene, bulletID, PROJECTILE_COMPONENT_ID, &projectile);
    Scene_Assign(scene, bulletID, BULLET_COMPONENT_ID, NULL);

    return bulletID;
}

EntityID AirBullet_Create(struct scene* scene, Vector pos, Vector tar, float attack, Nation* nation)
{
    EntityID airBulletID = Bullet_Create(scene, pos, tar, attack, nation);
    Scene_Unassign(scene, airBulletID, BULLET_COMPONENT_ID);
    Scene_Assign(scene, airBulletID, AIR_BULLET_COMPONENT_ID, NULL);
    Scene_Unassign(scene, airBulletID, BUILDING_LAYER_COMPONENT_ID);
    Scene_Assign(scene, airBulletID, AIR_LAYER_COMPONENT_ID, NULL);
    ((Sprite*)Scene_GetComponent(scene, airBulletID, SPRITE_COMPONENT_ID))->z = 1.0f;
    ((Sprite*)Scene_GetComponent(scene, airBulletID, SPRITE_COMPONENT_ID))->aZ = 0;
    return airBulletID;
}