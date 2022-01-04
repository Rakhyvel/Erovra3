#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Coal_Create(struct scene* scene, Vector pos, Nation* nation)
{
    if (nation->capital == INVALID_ENTITY_INDEX) { // If the nation is defeated, no coal for you!
        return INVALID_ENTITY_INDEX;
    }
    EntityID coalID = Scene_NewEntity(scene);

    Sprite* capitalSprite = (Sprite*)Scene_GetComponent(scene, nation->capital, SPRITE_COMPONENT_ID);
    Vector vel = Vector_Sub(capitalSprite->pos, pos);
    vel = Vector_Normalize(vel);
    vel = Vector_Scalar(vel, min(6, Vector_Dist(pos, capitalSprite->pos) / 16.0f));
    float angle = Vector_Angle(vel);
    Sprite sprite = {
        COAL_TEXTURE_ID,
        NULL,
        COAL_SHADOW_TEXTURE_ID,
        nation,
        pos,
        vel,
        0.5f,
        0,
        0.2f,
        0,
        0,
        RenderPriorirty_PARTICLE_LAYER,
        20,
        20,
        0,
        true,
        nation->controlFlag == AI_COMPONENT_ID,
        false,
    };
    Scene_Assign(scene, coalID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, coalID, PARTICLE_LAYER_COMPONENT_ID, NULL);

    ResourceParticle resourceParticle = {
        ResourceType_COAL,
        Vector_Dist(pos, capitalSprite->pos),
        capitalSprite->pos
    };
    Scene_Assign(scene, coalID, RESOURCE_PARTICLE_COMPONENT_ID, &resourceParticle);

    Scene_Assign(scene, coalID, nation->controlFlag, NULL);

    return coalID;
}