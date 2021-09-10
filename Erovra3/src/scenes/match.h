#pragma once
#include "../components/components.h"
#include "../engine/scene.h"
#include "../terrain.h"
#include "../util/lexicon.h"

extern Terrain* terrain;

struct message {
    char text[255];
    int fade;
    SDL_Color color;
};

extern const int ticksPerLabor; // 400 = standard; 240 = unit/min
extern bool buildPorts;

EntityID ENGINEER_FOCUSED_GUI;

// Default containers
EntityID BUILDING_FOCUSED_GUI;
EntityID UNIT_FOCUSED_GUI;

EntityID FACTORY_READY_FOCUSED_GUI;
EntityID FACTORY_BUSY_FOCUSED_GUI;

EntityID PORT_READY_FOCUSED_GUI;
EntityID PORT_BUSY_FOCUSED_GUI;

EntityID ACADEMY_READY_FOCUSED_GUI;
EntityID ACADEMY_BUSY_FOCUSED_GUI;

bool Match_CityHasType(Scene* scene, City* city, UnitType type);
bool Match_PlaceOrder(Scene* scene, Nation* nation, Producer* producer, Expansion* expansion, UnitType type);
bool Match_BuyCity(struct scene* scene, EntityID nationID, Vector pos);
bool Match_BuyExpansion(struct scene* scene, UnitType type, EntityID nationID, Vector pos);

void Match_Render(Scene* scene);
Scene* Match_Init(float* map, char* capitalName, Lexicon* lexicon, int mapSize, bool AIControlled);