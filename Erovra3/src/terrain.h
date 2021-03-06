/*
terrain.h
*/

#pragma once
#include <SDL.h>
#include <stdbool.h>

#include "./util/vector.h"

typedef struct terrain {
    float* map;
    float* timber;
    float* coal;
    float* ore;
    unsigned short* continents;
    int numContinents;
    unsigned short* oceans;
    int numOceans;
    Arraylist* contOceanAdjacency;
    size_t size;
    size_t tileSize;
    SDL_Texture* texture;
    EntityID* buildings;
    EntityID* walls;
    Arraylist* ports;
} Terrain;

struct terrain* Terrain_Create(int mapSize, float* map, float* trees, float* ore, SDL_Texture* texture, int* status);
void Terrain_Destroy(struct terrain*);

SDL_Color Terrain_RealisticColor(float* map, int mapSize, int x, int y, float i);
SDL_Color Terrain_MiniMapColor(float* map, int mapSize, int x, int y, float i);

void Terrain_Update(struct terrain* terrain);
void Terrain_Render(struct terrain* terrain);

// Continent stuff
void Terrain_FindPorts(struct terrain* terrain);
void Terrain_FindContinentsAndOceans(struct terrain* terrain, int* status);
void Terrain_EliminateUselessPortPoints(struct terrain* terrain, Arraylist* capitalPoints);

// Some map functions
float Terrain_GetHeight(struct terrain*, int x, int y);
void Terrain_SetOffset(struct vector);
float Terrain_GetZoom();
bool Terrain_LineOfSight(struct terrain* terrain, Vector from, Vector to, float z);
Vector Terrain_LineOfSightPoint(struct terrain* terrain, Vector from, Vector to, float z);

// Resources
float Terrain_GetOre(struct terrain*, int x, int y);
float Terrain_GetCoal(struct terrain* terrain, int x, int y);
float Terrain_GetTimber(struct terrain* terrain, int x, int y);

// Building map functions
float Terrain_GetHeightForBuilding(struct terrain*, int x, int y);
EntityID Terrain_GetBuildingAt(struct terrain*, int x, int y);
void Terrain_SetBuildingAt(struct terrain*, EntityID id, int x, int y);
int Terrain_ClosestBuildingDist(struct terrain* terrain, int x1, int y1);
int Terrain_ClosestMaskDist(struct scene* scene, ComponentKey key, struct terrain* terrain, int x1, int y1);
EntityID Terrain_AdjacentMask(struct scene* scene, ComponentKey key, struct terrain* terrain, int x1, int y1);

// Wall functions
EntityID Terrain_GetWallAt(struct terrain*, int x, int y);
void Terrain_SetWallAt(struct terrain*, EntityID id, int x, int y);

// Map affine transformations
void Terrain_Translate(SDL_Rect* newPos, float x, float y, float width, float height);
SDL_Point Terrain_InverseTranslate(int x, int y);
struct vector Terrain_MousePos();

// Some color mod. functions
bool Terrain_IsBorder(float* terrain, int width, int height, int x, int y, float z, int i);
SDL_Color Terrain_HSVtoRGB(float hue, float sat, float val);

bool Terrain_IsSolidSquare(struct terrain* terrain, Vector point);
struct vector Terrain_FindBestLocation(struct terrain* terrain, struct vector start);