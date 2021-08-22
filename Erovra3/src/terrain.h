/*
terrain.h
*/

#pragma once
#include <SDL.h>
#include <stdbool.h>

#include "./util/vector.h"

typedef struct terrain {
    float* map;
    float* ore;
    int size;
    int tileSize;
    SDL_Texture* texture;
    EntityID* buildings;
    EntityID* walls;
} Terrain;

typedef struct gradient {
    float gradX;
    float gradY;
    float z; // Optional
} Gradient;

struct terrain* Terrain_Create(int tileSize, float* map, SDL_Texture* texture);
void Terrain_Destroy(struct terrain*);

void Terrain_PaintMap(int size, float* map, SDL_Texture* texture);
void Terrain_Update(struct terrain* terrain);
void Terrain_Render(struct terrain* terrain);

// Interpolations
float Terrain_LinearInterpolation(int x0, float y0, int x1, float y1, float x2);
float Terrain_BilinearInterpolation(int x0, int y0, int cellSize, float* map, int mapWidth, float x2, float y2);

float Terrain_CosineInterpolation(int x0, float y0, int x1, float y1, float x2);
float Terrain_BicosineInterpolation(int x0, int y0, int cellSize, float* map, int mapWidth, float x2, float y2);

// Height map generators
float* Terrain_Generate(int mapSize, int cellSize, float amplitude, unsigned int seed);
float* Terrain_Perlin(int mapSize, int cellSize, unsigned int seed, int* status);

// Map modifications
void Terrain_Normalize(float* map, int mapSize);
void Terrain_Erode(int size, float* map, float intensity, int* status);

// Some map functions
float Terrain_GetHeight(struct terrain*, int x, int y);
void Terrain_SetOffset(struct vector);
Gradient Terrain_GetGradient(int size, float* map, float posX, float posY);
float Terrain_GetZoom();
bool Terrain_LineOfSight(struct terrain* terrain, Vector from, Vector to, float z);
Vector Terrain_LineOfSightPoint(struct terrain* terrain, Vector from, Vector to);

// Resources
float Terrain_GetOre(struct terrain*, int x, int y);

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
struct vector Terrain_MousePos();

// Some color mod. functions
bool Terrain_IsBorder(float* terrain, int width, int height, int x, int y, float z, int i);
SDL_Color Terrain_HSVtoRGB(float hue, float sat, float val);

bool Terrain_IsSolidSquare(struct terrain* terrain, Vector point);
struct vector Terrain_FindBestLocation(struct terrain* terrain, struct vector start);