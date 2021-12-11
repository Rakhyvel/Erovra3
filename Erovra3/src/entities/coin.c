#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Coin_Create(struct scene* scene, Vector pos, EntityID nationID)
{
    Nation* nation = (Nation*)Scene_GetComponent(scene, nationID, NATION_COMPONENT_ID);
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
        pos,
        0.5f,
        vel,
        0,
        0.2f,
        true,
        0.01, // Z vel
        -0.0001f, // z acc due to gravity
        COIN_TEXTURE_ID,
        NULL,
        COIN_SHADOW_TEXTURE_ID,
        RenderPriorirty_PARTICLE_LAYER,
        nation->controlFlag == AI_COMPONENT_ID,
        false,
        nationID,
        20,
        20,
        0,
        0
    };
    Scene_Assign(scene, coinID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, coinID, PARTICLE_LAYER_COMPONENT_ID, NULL);

    ResourceParticle resourceParticle = {
        ResourceType_COIN,
        Vector_Dist(pos, capitalSprite->pos),
        capitalSprite->pos
    };
    Scene_Assign(scene, coinID, RESOURCE_PARTICLE_COMPONENT_ID, &resourceParticle);

    Scene_Assign(scene, coinID, GET_COMPONENT_FIELD(scene, nationID, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return coinID;
}