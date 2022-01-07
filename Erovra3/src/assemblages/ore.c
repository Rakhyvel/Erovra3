#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./assemblages.h"

EntityID Ore_Create(struct scene* scene, Vector pos, Nation* nation, EntityID accepter)
{
    if (nation->capital == INVALID_ENTITY_INDEX) { // If the nation is defeated, no ore for you!
        return INVALID_ENTITY_INDEX;
    }
    EntityID oreID = Scene_NewEntity(scene);

    Sprite* accepterSprite = (Sprite*)Scene_GetComponent(scene, accepter, SPRITE_COMPONENT_ID);
    Vector vel = Vector_Sub(accepterSprite->pos, pos);
    vel = Vector_Normalize(vel);
    vel = Vector_Scalar(vel, min(6, Vector_Dist(pos, accepterSprite->pos) / 16.0f));
    float angle = Vector_Angle(vel);
    Sprite sprite = {
        ORE_TEXTURE_ID,
        NULL,
        ORE_SHADOW_TEXTURE_ID,
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
    Scene_Assign(scene, oreID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, oreID, PARTICLE_LAYER_COMPONENT_ID, NULL);

    ResourceParticle resourceParticle = {
        ResourceType_ORE,
        Vector_Dist(pos, accepterSprite->pos),
        accepterSprite->pos,
        accepter
    };
    Scene_Assign(scene, oreID, RESOURCE_PARTICLE_COMPONENT_ID, &resourceParticle);

    // Increment transit for resource particle
    ResourceAccepter* resourceAccepter = (ResourceAccepter*)Scene_GetComponent(scene, resourceParticle.accepter, RESOURCE_ACCEPTER_COMPONENT_ID);
    resourceAccepter->transit[resourceParticle.type]++;

    Scene_Assign(scene, oreID, nation->controlFlag, NULL);

    return oreID;
}