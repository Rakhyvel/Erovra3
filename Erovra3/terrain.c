#pragma once
#include <SDL.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>

#include "game.h"
#include "terrain.h"
#include "vector.h"

float* map;
float* ore;
int size;
bool mouseDown = false;
struct vector offset = { 0, 0 };
struct vector oldOffset = { 0, 0 };
float terrain_zoom = 1;
int oldWheel = 0;

struct terrain* terrain_create(int mapSize, struct game* g) {
	struct terrain* retval = malloc(sizeof(struct terrain));
	size = mapSize;
	map = terrain_perlin(size, size / 2);
	ore = terrain_perlin(size / 64, size / 128);
	terrain_normalize(map, size);
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			map[x + y * size] = map[x + y * size] * 0.7f + 0.15;
		}
	}
	paintMap(retval, g);
	return retval;
}

void paintMap(struct terrain* terrain, struct game* g) {
	terrain->texture = SDL_CreateTexture(g->rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, size, size);
	terrain->pixels = malloc(size * size * 4);
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			const unsigned int offset = (size * 4 * y) + x * 4;
			SDL_Color terrainColor;
			float i = terrain_getHeight(x, y);
			i = i * 2 - 0.5f;
			if (i < 0.5) {
				// water
				i *= 2;
				i = i * i * i;
				terrainColor = terrain_HSVtoRGB(210.0f - 15.0f * i, 0.90f - 0.5f * i, 0.35f + 0.35f * i);
			}
			else {
				i = (i - 0.5f) * 2;
				i = sqrt(i);
				terrainColor = terrain_HSVtoRGB(30.0f + 100.0f * i, (i * 0.1f) + 0.25f, 0.7f - i * 0.35f);
			}
			if (terrain_isBorder(map, size, size, x, y, 0.5f, 1)) {
				terrainColor = (SDL_Color){ 255, 255, 255 };
			}

			terrain->pixels[offset + 0] = terrainColor.b;
			terrain->pixels[offset + 1] = terrainColor.g;
			terrain->pixels[offset + 2] = terrainColor.r;
			terrain->pixels[offset + 3] = SDL_ALPHA_OPAQUE;
		}
	}
	SDL_UpdateTexture(terrain->texture, NULL, terrain->pixels, size * 4);
}

void terrain_render(struct terrain* terrain, struct game* g) {
	SDL_Rect rect = { 0, 0 };
	rect.w = rect.h = (int)(terrain_zoom * size);
	SDL_RenderCopy(g->rend, terrain->texture, NULL, &rect);

	SDL_SetRenderDrawColor(g->rend, 0, 0, 0, 50);
	SDL_FPoint gridLineStart = { 32, 32 };
	SDL_Rect gridLineRect = { 0, 0 };
	for (int x = 0; x <= size / 64; x++) {
		SDL_RenderDrawLine(g->rend,
			(int)(gridLineRect.x + x * 64.0 * terrain_zoom),
			gridLineRect.y,
			(int)(gridLineRect.x + x * 64.0 * terrain_zoom),
			(int)(gridLineRect.y + (size * terrain_zoom)));
	}
	for (int y = 0; y <= size / 64; y++) {
		SDL_RenderDrawLine(g->rend,
			gridLineRect.x,
			(int)(gridLineRect.y + y * 64.0 * terrain_zoom),
			(int)(gridLineRect.x + (size * terrain_zoom)),
			(int)(gridLineRect.y + y * 64.0 * terrain_zoom));
	}
}

/*
 Does linear interpolation, basically draws a line between two points, and
 finds the y value to the corresponding x value

 Parameters: int x0 - The x coordinate of the first point
			 int y0 - The y coordinate of the first point
			 int x1 - The x coordinate of the second point
			 int y1 - The y coordinate of the second point
			 int x2 - The x coordinate of the unknown point

 Returns:	 The y value of the unknown point (x2, y?)
 */
float terrain_linearInterpolation(int x0, float y0, int x1, float y1, float x2) {
	float lol = (float)y0 + (float)(x2 - x0) * (float)(y1 - y0) / (float)(x1 - x0);
	return lol;
}

/*
Does bilinear interpolation. Does linear interpolation on top and bottom edges, then does linear interpolation
between those two.

Parameters: int x0 - The x-coordinate of the top-right corner of the cell
			int y0 - The y-cooridnate of the top-right corner of the cell
			int cellSize - The height and width of the cell (cells are squares)
			float* map - Pointer to the map
			int mapWidth - The height and width of the map (maps are squares)
			float x2 - The x coordinate of the point within the cell to interpolate
			float y2 - The y coordinate of the point within the cell to interpolate

Returns:    The z value at the specified point (x2, y2)
*/
float terrain_bilinearInterpolation(int x0, int y0, int cellSize, float* map, int mapWidth, float x2, float y2) {
	int x1 = x0 + cellSize;
	int y1 = y0 + cellSize;
	if (x1 >= mapWidth) {
		x1 = 0;
	}
	if (y1 >= mapWidth) {
		y1 = 0;
	}

	float top = terrain_linearInterpolation(x0, map[y0 * mapWidth + x0], x0 + cellSize, map[y0 * mapWidth + x1], x2);
	float bottom = terrain_linearInterpolation(x0, map[y1 * mapWidth + x0], x0 + cellSize, map[y1 * mapWidth + x1], x2);

	return terrain_linearInterpolation(y0, top, y0 + cellSize, bottom, y2);
}

/*
* Performs the taylor series expansion for cosine. Accurate for range {-pi < x < pi}
*/
double fastCos(double x) {
	if (x > 3.14159265359) {
		x -= 2 * 3.14159265359;
	}
	else if (x < -3.14159265359) {
		x += 2 * 3.14159265359;
	}
	double x2 = x * x;
	double x4 = x2 * x2;
	double x6 = x4 * x2;
	double x8 = x6 * x2;
	double x10 = x8 * x2;
	// Inverse factorials
	double fact2 = 0.5;
	double fact4 = 0.04166666666;
	double fact6 = 0.00138888888;
	double fact8 = 0.00002480158;
	double fact10 = 2.75573192E-7;

	return 1.0 - x2 * fact2 + x4 * fact4 - x6 * fact6 + x8 * fact8 - x10 * fact10;
}

float terrain_cosineInterpolation(int x0, float y0, int x1, float y1, float x) {
	double xDiff = (double)(x1 - x0);
	double mu2 = (1 - fastCos((double)(x - x0) * 3.141592653589793 / xDiff)) / 2;
	double retval = (y0 * (1 - mu2) + y1 * mu2);
	return (float)retval;
}

float terrain_bicosineInterpolation(int x0, int y0, int cellSize, float* map, int mapWidth, float x2, float y2) {
	int x1 = x0 + cellSize;
	int y1 = y0 + cellSize;
	if (x1 >= mapWidth) {
		x1 = 0;
	}
	if (y1 >= mapWidth) {
		y1 = 0;
	}

	float top = terrain_cosineInterpolation(x0, map[y0 * mapWidth + x0], x0 + cellSize, map[(int)(y0 * mapWidth + x1)], x2);
	float bottom = terrain_cosineInterpolation(x0, map[y1 * mapWidth + x0], x0 + cellSize, map[(int)(y1 * mapWidth + x1)], x2);

	return terrain_cosineInterpolation(y0, top, y0 + cellSize, bottom, y2);
}

/*
Generates a random height map of a given size, that has cells of a given size, and a given maximum/minimum amplitude

Parameters: int mapSize - Height and width of map (maps are squares)
			int cellSize - Height and width of cells. Ought to be a factor of mapSize.
			float amplitude - Max/min amplitude of values in map

Returns: A pointer to the begining of the map. Map is row major ordered as an array of floats of size mapSize * mapSize.
		 Callee is responsible for freeing map.
*/
float* terrain_generate(int mapSize, int cellSize, float amplitude) {
	// Allocate map
	float* retval = (float*)malloc(mapSize * mapSize * sizeof(float));

	// Sparsely fill with random values according to cell size
	for (int y = 0; y < mapSize; y += cellSize) {
		for (int x = 0; x < mapSize; x += cellSize) {
			retval[y * mapSize + x] = amplitude * (0.5 * (float)rand() / (float)RAND_MAX);
		}
	}
	// Interpolate the rest of the map
	for (int i = 0; i < mapSize * mapSize; i++) {
		int x = i % mapSize; // x coord of point
		int y = i / mapSize; // y coord of point
		int x1 = (int)((x / cellSize) * cellSize); // x coord of point's cell
		int y1 = (int)((y / cellSize) * cellSize); // y coord of point's cell

		if (x != x1 || y != y1) {
			retval[x + y * mapSize] = terrain_bicosineInterpolation(x1, y1, cellSize, retval, mapSize, (float)x, (float)y);
		}
	}
	return retval;
}

/**
Generates a perlin noise map. Map has fractal detail, so detail on higher
scales and detail on lower scales. This makes it good for terrain

Parameters: int mapSize - Height and width of the map (maps are squares)
			int cellSize - Size of cells. Lower values = lower grain, higher values = higher grain

Returns: a pointer to a float array, with size of mapSize * mapSize, in row major order.
*/
float* terrain_perlin(int mapSize, int cellSize) {
	if (cellSize < 1) {
		cellSize = mapSize / 8;
	}
	float amplitude = 1;
	float* retval = terrain_generate(mapSize, cellSize, amplitude);
	cellSize /= 2;
	amplitude /= 2;

	while (cellSize > 2) {
		float* map = terrain_generate(mapSize, cellSize, amplitude);
		for (int i = 0; i < mapSize * mapSize; i++) {
			retval[i] += map[i];
		}
		free(map);

		cellSize *= 0.5f;
		amplitude *= 0.5f;
	}
	return retval;
}


/*
Normalizes a map so that the lowest value is 0, highest value is 1, and the average value is 0.5

Parameters: float* map - map to normalize
			int mapSize - height and width of map (maps are squares)
*/
void terrain_normalize(float* map, int mapSize) {
	float maxValue = FLT_MIN;
	float minValue = FLT_MAX;
	float average = 0;
	int number = 0;

	// Find maximum and minimum values
	for (int x = 0; x < mapSize; x++) {
		for (int y = 0; y < mapSize; y++) {
			maxValue = max(map[x + y * mapSize], maxValue);
			minValue = min(map[x + y * mapSize], minValue);
		}
	}

	// Find average value
	for (int x = 0; x < mapSize; x++) {
		for (int y = 0; y < mapSize; y++) {
			map[x + y * mapSize] = (float)(map[x + y * mapSize] - minValue) / (float)(maxValue - minValue);
			average += map[x + y * mapSize];
			number++;
		}
	}
	average = 0.5f - (average / (float)number);

	// Update values
	for (int x = 0; x < mapSize; x++) {
		for (int y = 0; y < mapSize; y++) {
			map[x + y * mapSize] = map[x + y * mapSize] += average;
		}
	}
}

float terrain_getHeight(int x, int y) {
	return map[y * size + x];
}

float terrain_getZoom() {
	return terrain_zoom;
}

/*
This checks to see if a given point is a border between two values
*/
bool terrain_isBorder(float* terrain, int width, int height, int x, int y, float z, int i) {
	bool containsWater = terrain[x + y * width] <= z;
	if (x > 0)
		containsWater |= terrain[x - i + y * width] <= z;
	if (x < width - 1)
		containsWater |= terrain[x + i + y * width] <= z;
	if (y > 0)
		containsWater |= terrain[x + (y - i) * width] <= z;
	if (y < height - 1)
		containsWater |= terrain[x + (y + i) * width] <= z;

	bool containsLand = terrain[x + y * width] >= z;
	if (x > 0)
		containsLand |= terrain[x - i + y * width] >= z;
	if (x < width - 1)
		containsLand |= terrain[x + i + y * width] >= z;
	if (y > 0)
		containsLand |= terrain[x + (y - i) * width] >= z;
	if (y < height - 1)
		containsLand |= terrain[x + (y + i) * width] >= z;

	return containsLand && containsWater;
}

/*
Takes in HSV values and gives back an SDL_Color which is RGB
*/
SDL_Color terrain_HSVtoRGB(float hue, float saturation, float value)
{
	hue += 360;
	hue = (float)fmod(hue, 360);
	saturation = max(0.0f, min(1.0f, saturation));
	value = max(0.0f, min(1.0f, value));
	double c = value * saturation;
	double x = c * (1 - fabs(fmod((hue / 60), 2) - 1));
	double m = value - c;
	double r = 0, g = 0, b = 0;

	if (hue >= 0 && hue < 60) {
		r = c;
		g = x;
		b = 0;
	}
	else if (hue >= 60 && hue < 120) {
		r = x;
		g = c;
		b = 0;
	}
	else if (hue >= 120 && hue < 180) {
		r = 0;
		g = c;
		b = x;
	}
	else if (hue >= 180 && hue < 240) {
		r = 0;
		g = x;
		b = c;
	}
	else if (hue >= 240 && hue < 300) {
		r = x;
		g = 0;
		b = c;
	}
	else if (hue >= 300 && hue < 360) {
		r = c;
		g = 0;
		b = x;
	}
	SDL_Color color = { (Uint8)((r + m) * 255), (Uint8)((g + m) * 255), (Uint8)((b + m) * 255) };
	return color;
}