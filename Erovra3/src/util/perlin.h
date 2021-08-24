#ifndef PERLIN_H
#define PERLIN_H

#include <SDL.h>

typedef struct gradient {
    float gradX;
    float gradY;
    float z; // Optional
} Gradient;

enum PerlinInterpolation {
	BILINEAR,
	BICOSINE
};


// Interlopate
float Perlin_LinearInterpolation(int x0, float y0, int x1, float y1, float x2);
float Perlin_BilinearInterpolation(float* map, int mapSize, int cellSize, int x0, int y0, float x2, float y2);

float Perlin_CosineInterpolation(int x0, float y0, int x1, float y1, float x2);
float Perlin_BicosineInterpolation(float* map, int mapSize, int cellSize, int x0, int y0, float x2, float y2);

// Height map generators
void Perlin_GenerateOctave(float* map, int mapSize, int cellSize, float amplitude, unsigned int seed, enum PerlinInterpolation interpolation);
float* Perlin_Generate(int mapSize, int cellSize, unsigned int seed, int* status);

// Map modifications
void Perlin_Normalize(float* map, int mapSize);
Gradient Perlin_GetGradient(float* map, int mapSize, float posX, float posY);
void Perlin_Erode(float* map, int mapSize, float intensity, int* status);

// Map painting
void Perlin_PaintMap(float* map, int mapSize, SDL_Texture* texture, SDL_Color(colorFunction)(float* map, int mapSize, int x, int y, float i));

#endif