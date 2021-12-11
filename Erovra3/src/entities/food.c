#include "../scenes/match.h"
#include "../textures.h"
#include "./components.h"
#include "./entities.h"

EntityID Food_Create(struct scene* scene, Vector pos, EntityID nationID)
{
    Nation* nation = (Nation*)Scene_GetComponent(scene, nationID, NATION_COMPONENT_ID);
    if (nation->capital == INVALID_ENTITY_INDEX) { // If the nation is defeated, no food for you!
        return INVALID_ENTITY_INDEX;
    }
    EntityID foodID = Scene_NewEntity(scene);

    Motion* capitalMotion = (Motion*)Scene_GetComponent(scene, nation->capital, MOTION_COMPONENT_ID);
    Vector vel = Vector_Sub(capitalMotion->pos, pos);
    vel = Vector_Normalize(vel);
    vel = Vector_Scalar(vel, 6);
    float angle = Vector_Angle(vel);
    Motion motion = {
        pos,
        0.5f,
        vel,
        0,
        0.2f,
        true
    };
    Scene_Assign(scene, foodID, MOTION_COMPONENT_ID, &motion);

    SimpleRenderable render = {
        ORE_TEXTURE_ID,
        NULL,
        ORE_SHADOW_TEXTURE_ID,
        RenderPriorirty_PARTICLE_LAYER,
        nation->controlFlag == AI_COMPONENT_ID,
        false,
        nationID,
        20,
        20,
        0,
        0
    };
    Scene_Assign(scene, foodID, SIMPLE_RENDERABLE_COMPONENT_ID, &render);
    Scene_Assign(scene, foodID, PARTICLE_LAYER_COMPONENT_ID, NULL);

    ResourceParticle resourceParticle = {
        ResourceType_FOOD,
        Vector_Dist(pos, capitalMotion->pos),
        capitalMotion->pos
    };
    Scene_Assign(scene, foodID, RESOURCE_PARTICLE_COMPONENT_ID, &resourceParticle);

    Scene_Assign(scene, foodID, GET_COMPONENT_FIELD(scene, nationID, NATION_COMPONENT_ID, Nation, controlFlag), NULL);

    return foodID;
}