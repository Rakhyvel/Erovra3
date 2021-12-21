#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Coin_Create(struct scene* scene, Vector pos, Nation* nation)
{
    if (nation->capital == INVALID_ENTITY_INDEX) {
        return INVALID_ENTITY_INDEX;
    }
    EntityID coinID = Scene_NewEntity(scene);

    Sprite* capitalSprite = (Sprite*)Scene_GetComponent(scene, nation->capital, SPRITE_COMPONENT_ID);
    Vector vel = Vector_Sub(capitalSprite->pos, pos);
    vel = Vector_Normalize(vel);
    vel = Vector_Scalar(vel, 6);
    float angle = Vector_Angle(vel);
    Sprite sprite = {
        COIN_TEXTURE_ID, //	sprite
        NULL, // spriteOutline
        COIN_SHADOW_TEXTURE_ID, // shadow
        nation, // nation
        pos, // pos
        vel, // vel
        0.5f, // z
        0, // angle
        0.2f, // speed
        0.01, // Z vel
        -0.0001f, // z acc due to gravity
        RenderPriorirty_PARTICLE_LAYER, // priority
        20,
        20,
        0,
        true,
        nation->controlFlag == AI_COMPONENT_ID,
        false,
    };
    Scene_Assign(scene, coinID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, coinID, PARTICLE_LAYER_COMPONENT_ID, NULL);

    ResourceParticle resourceParticle = {
        ResourceType_COIN,
        Vector_Dist(pos, capitalSprite->pos),
        capitalSprite->pos
    };
    Scene_Assign(scene, coinID, RESOURCE_PARTICLE_COMPONENT_ID, &resourceParticle);

    Scene_Assign(scene, coinID, nation->controlFlag, NULL);

    return coinID;
}