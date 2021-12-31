#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Food_Create(struct scene* scene, Vector pos, Nation* nation)
{
    if (nation->capital == INVALID_ENTITY_INDEX) { // If the nation is defeated, no food for you!
        return INVALID_ENTITY_INDEX;
    }
    EntityID foodID = Scene_NewEntity(scene);

    Sprite* capitalSprite = (Sprite*)Scene_GetComponent(scene, nation->capital, SPRITE_COMPONENT_ID);
    Vector vel = Vector_Sub(capitalSprite->pos, pos);
    vel = Vector_Normalize(vel);
    vel = Vector_Scalar(vel, min(6, Vector_Dist(pos, capitalSprite->pos) / 16.0f));
    float angle = Vector_Angle(vel);
    Sprite sprite = {
        ORE_TEXTURE_ID, // sprite
        NULL, // spriteOutline
        ORE_SHADOW_TEXTURE_ID, // shadow
        nation, // nation
        pos, // pos
        vel, // vel
        0.5f, // z
        0, // angle
        0.2f, // speed
        0, // dz
        0, // az
        RenderPriorirty_PARTICLE_LAYER,
        20,
        20,
        0,
        true,
        nation->controlFlag == AI_COMPONENT_ID,
        false,
    };
    Scene_Assign(scene, foodID, SPRITE_COMPONENT_ID, &sprite);
    Scene_Assign(scene, foodID, PARTICLE_LAYER_COMPONENT_ID, NULL);

    ResourceParticle resourceParticle = {
        ResourceType_FOOD,
        Vector_Dist(pos, capitalSprite->pos),
        capitalSprite->pos
    };
    Scene_Assign(scene, foodID, RESOURCE_PARTICLE_COMPONENT_ID, &resourceParticle);

    Scene_Assign(scene, foodID, nation->controlFlag, NULL);

    return foodID;
}