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

struct terrain* terrain_create(int tileSize, float biome, int scale);
void paintMap(struct terrain* terrain);
void terrain_render(struct terrain* terrain);

// Interpolations
float terrain_linearInterpolation(int x0, float y0, int x1, float y1, float x2);
float terrain_bilinearInterpolation(int x0, int y0, int cellSize, float* map, int mapWidth, float x2, float y2);

float terrain_cosineInterpolation(int x0, float y0, int x1, float y1, float x2);
float terrain_bicosineInterpolation(int x0, int y0, int cellSize, float* map, int mapWidth, float x2, float y2);

// Height map generators
float* terrain_generate(int mapSize, int cellSize, float amplitude);
float* terrain_perlin(int mapSize, int cellSize);

// Map modifications
void terrain_normalize(float* map, int mapSize);
void terrain_erode(struct terrain* terrain);

// Some map functions
float terrain_getHeight(struct terrain*, int x, int y);
void terrain_setOffset(struct vector);
Gradient terrain_getGradient(struct terrain* terrain, float posX, float posY);
float terrain_getZoom();
bool terrain_lineOfSight(struct terrain* terrain, Vector from, Vector to, float z);
Vector terrain_lineOfSightPoint(struct terrain* terrain, Vector from, Vector to);

// Resources
float terrain_getOre(struct terrain*, int x, int y);

// Building map functions
float terrain_getHeightForBuilding(struct terrain*, int x, int y);
EntityID terrain_getBuildingAt(struct terrain*, int x, int y);
void terrain_addBuildingAt(struct terrain*, EntityID id, int x, int y);
int terrain_closestBuildingDist(struct terrain* terrain, int x1, int y1);
int terrain_closestMaskDist(struct scene* scene, ComponentMask mask, struct terrain* terrain, int x1, int y1);
EntityID terrain_adjacentMask(struct scene* scene, ComponentMask mask, struct terrain* terrain, int x1, int y1);

// Wall functions
EntityID terrain_getWallAt(struct terrain*, int x, int y);
void terrain_addWallAt(struct terrain*, EntityID id, int x, int y);

// Map affine transformations
void terrain_translate(SDL_FRect* newPos, float x, float y, float width, float height);
struct vector Terrain_MousePos();
struct vector terrain_translateVector(float x, float y);

// Some color mod. functions
bool terrain_isBorder(float* terrain, int width, int height, int x, int y, float z, int i);
SDL_Color terrain_HSVtoRGB(float hue, float sat, float val);

bool terrain_isSolidSquare(struct terrain* terrain, Vector point);
struct vector findBestLocation(struct terrain* terrain, struct vector start);