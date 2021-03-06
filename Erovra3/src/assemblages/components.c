#include "./components.h"
#include "../engine/scene.h"

void Components_Init()
{
    SPRITE_COMPONENT_ID = rand();
    TARGET_COMPONENT_ID = rand();
    PATROL_COMPONENT_ID = rand();
    BUILDING_LAYER_COMPONENT_ID = rand();
    SURFACE_LAYER_COMPONENT_ID = rand();
    AIR_LAYER_COMPONENT_ID = rand();
    PLANE_LAYER_COMPONENT_ID = rand();
    PARTICLE_LAYER_COMPONENT_ID = rand();

    UNIT_COMPONENT_ID = rand();
    BOARDABLE_COMPONENT_ID = rand();
    COMBATANT_COMPONENT_ID = rand();
    GROUND_UNIT_FLAG_COMPONENT_ID = rand();
    LAND_UNIT_FLAG_COMPONENT_ID = rand();
    ENGINEER_UNIT_FLAG_COMPONENT_ID = rand();
    BUILDING_FLAG_COMPONENT_ID = rand();
    WALL_FLAG_COMPONENT_ID = rand();
    BULLET_ATTACK_FLAG_COMPONENT_ID = rand();
    SHELL_ATTACK_FLAG_COMPONENT_ID = rand();
    SHIP_FLAG_COMPONENT_ID = rand();
    AIRCRAFT_FLAG_COMPONENT_ID = rand();

    PROJECTILE_COMPONENT_ID = rand();
    BULLET_COMPONENT_ID = rand();
    AIR_BULLET_COMPONENT_ID = rand();
    BOMB_COMPONENT_ID = rand();
    SHELL_COMPONENT_ID = rand();

    CITY_COMPONENT_ID = rand();
    PRODUCER_COMPONENT_ID = rand();
    EXPANSION_COMPONENT_ID = rand();

    RESOURCE_PARTICLE_COMPONENT_ID = rand();
    RESOURCE_PRODUCER_COMPONENT_ID = rand();
    RESOURCE_ACCEPTER_COMPONENT_ID = rand();

    PLAYER_FLAG_COMPONENT_ID = rand();
    AI_COMPONENT_ID = rand();

    ORDER_BUTTON_COMPONENT_ID = rand();
    UNIT_LIST_COMPONENT_ID = rand();
}

void Components_Register(struct scene* scene)
{
    int i = sizeof(Nation);
    Scene_RegisterComponent(scene, SPRITE_COMPONENT_ID, sizeof(Sprite));
    Scene_RegisterComponent(scene, TARGET_COMPONENT_ID, sizeof(Target));
    Scene_RegisterComponent(scene, PATROL_COMPONENT_ID, sizeof(Patrol));
    Scene_RegisterComponent(scene, BUILDING_LAYER_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, SURFACE_LAYER_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, AIR_LAYER_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, PLANE_LAYER_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, PARTICLE_LAYER_COMPONENT_ID, 0);

    Scene_RegisterComponent(scene, UNIT_COMPONENT_ID, sizeof(Unit));
    Scene_RegisterComponent(scene, BOARDABLE_COMPONENT_ID, sizeof(Boardable));
    Scene_RegisterComponent(scene, COMBATANT_COMPONENT_ID, sizeof(Combatant));
    Scene_RegisterComponent(scene, GROUND_UNIT_FLAG_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, LAND_UNIT_FLAG_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, ENGINEER_UNIT_FLAG_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, BUILDING_FLAG_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, WALL_FLAG_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, BULLET_ATTACK_FLAG_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, SHELL_ATTACK_FLAG_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, SHIP_FLAG_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, AIRCRAFT_FLAG_COMPONENT_ID, 0);

    Scene_RegisterComponent(scene, PROJECTILE_COMPONENT_ID, sizeof(Projectile));
    Scene_RegisterComponent(scene, BULLET_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, AIR_BULLET_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, BOMB_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, SHELL_COMPONENT_ID, 0);

    Scene_RegisterComponent(scene, CITY_COMPONENT_ID, sizeof(City));
    Scene_RegisterComponent(scene, PRODUCER_COMPONENT_ID, sizeof(Producer));
    Scene_RegisterComponent(scene, EXPANSION_COMPONENT_ID, sizeof(Expansion));

    Scene_RegisterComponent(scene, RESOURCE_PARTICLE_COMPONENT_ID, sizeof(ResourceParticle));
    Scene_RegisterComponent(scene, RESOURCE_PRODUCER_COMPONENT_ID, sizeof(ResourceProducer));
    Scene_RegisterComponent(scene, RESOURCE_ACCEPTER_COMPONENT_ID, sizeof(ResourceAccepter));

    Scene_RegisterComponent(scene, PLAYER_FLAG_COMPONENT_ID, 0);
    Scene_RegisterComponent(scene, AI_COMPONENT_ID, 0);

    Scene_RegisterComponent(scene, ORDER_BUTTON_COMPONENT_ID, sizeof(OrderButton));
    Scene_RegisterComponent(scene, UNIT_LIST_COMPONENT_ID, 0);
}
