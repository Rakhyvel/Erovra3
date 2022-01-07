#include "../scenes/match.h"
#include "../textures.h"
#include "./assemblages.h"
#include "./components.h"

EntityID Metal_Create(struct scene* scene, Vector pos, Nation* nation, EntityID accepter)
{
    if (nation->capital == INVALID_ENTITY_INDEX) { // If the nation is defeated, no metal for you!
        return INVALID_ENTITY_INDEX;
    }
    EntityID metalID = Scene_NewEntity(scene);

    Sprite* capitalSprite = (Sprite*)Scene_GetComponent(scene, nation->capital, SPRITE_COMPONENT_ID);
    Vector vel = Vector_Sub(capitalSprite->pos, pos);
    vel = Vector_Normalize(vel);
    vel = Vector_Scalar(vel, min(6, Vector_Dist(pos, capitalSprite->pos) / 16.0f));
    float angle = Vector_Angle(vel);
    Sprite sprite = {
        METAL_TEXTURE_ID,
        NULL,
        METAL_SHADOW_TEXTURE_ID,
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
        false, //nation->controlFlag == AI_COMPONENT_ID,
        false,
    };
    Scene_Assign(scene, metalID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, metalID, PARTICLE_LAYER_COMPONENT_ID, NULL);

    ResourceParticle resourceParticle = {
        ResourceType_METAL,
        Vector_Dist(pos, capitalSprite->pos),
        capitalSprite->pos,
        accepter
    };
    Scene_Assign(scene, metalID, RESOURCE_PARTICLE_COMPONENT_ID, &resourceParticle);

    // Increment transit for resource particle
    ResourceAccepter* resourceAccepter = (ResourceAccepter*)Scene_GetComponent(scene, resourceParticle.accepter, RESOURCE_ACCEPTER_COMPONENT_ID);
    resourceAccepter->transit[resourceParticle.type]++;

    Scene_Assign(scene, metalID, nation->controlFlag, NULL);

    return metalID;
}