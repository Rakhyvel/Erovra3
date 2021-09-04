#pragma once
#include "../engine/scene.h"
#include "../terrain.h"
#include "../util/lexicon.h"

extern Terrain* terrain;

struct message {
    char text[255];
    int fade;
    SDL_Color color;
};

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

void Match_Render(Scene* scene);
Scene* Match_Init(float* map, char* capitalName, Lexicon* lexicon, int mapSize, bool AIControlled);