/*
terrain.h
*/

#pragma once
#include <SDL.h>
#include <stdbool.h>

#include "vector.h"

struct terrain {
	float* map;
	float* ore;
	int size;
	SDL_Texture* texture;
	Uint8* pixels; 
};

struct terrain* terrain_create(int);
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

// Some map functions
float terrain_getHeight(struct terrain*, int x, int y);
void terrain_setOffset(struct vector);
float terrain_getZoom();

// Map affine transformations
void terrain_translate(SDL_Rect* newPos, float x, float y, float width, float height);
SDL_Rect terrain_inverseTranslate(float x, float y, int width, int height);
struct vector terrain_translateVector(float x, float y);

// Some color mod. functions
bool terrain_isBorder(float* terrain, int width, int height, int x, int y, float z, int i);
SDL_Color terrain_HSVtoRGB(float hue, float sat, float val);