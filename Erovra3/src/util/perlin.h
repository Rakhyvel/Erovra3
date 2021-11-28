/*	perlin.h
* 
*	Contains functions and data structures relavant to creating perlin noise
*	heightmaps. Also contains functions for manipulating and eroding noise maps.
* 
*	@author	Joseph Shimel
*	@date	8/21/21
*/

#ifndef PERLIN_H
#define PERLIN_H

/**
 * @brief Used to represent a gradient on the map. Gives the direction of
 * steepest ascent. Also contains an optional z coordinate for convenience
*/
typedef struct gradient {
    float gradX;
    float gradY;
    float z; // Optional
} Gradient;

/**
 * @brief Used to differentiate between the two interpolation methods. Bilinear
 * is faster, while bicosine often looks more natural.
*/
enum PerlinInterpolation {
	BILINEAR,
	BICOSINE
};

/**
 * @brief Interpolates linearly between two points
 * @param x0 x coordinate of the first point
 * @param y0 y coordinate of the first point
 * @param x1 x coordinate of the second point
 * @param y1 y coordinate of the second point
 * @param x2 x coordinate of the point to interpolate
 * @return the corresponding y coordinate given the x coordinate, and the two points
*/
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
Gradient Perlin_GetSecondGradient(float* map, int mapSize, float posX, float posY);
void Perlin_Erode(float* map, int mapSize, float intensity, int* status);

#endif