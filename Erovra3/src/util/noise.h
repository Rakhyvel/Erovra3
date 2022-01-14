/*	noise.h
* 
*	Contains functions and data structures relavant to creating perlin noise
*	heightmaps. Also contains functions for manipulating and eroding noise maps.
* 
*	@author	Joseph Shimel
*	@date	8/21/21
*/

#ifndef PERLIN_H
#define PERLIN_H

#define M_PI 3.14159265358979323846

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
	BICOSINE,
	BICUBIC
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
float Noise_LinearInterpolation(int x0, float y0, int x1, float y1, float x2);
void Noise_BilinearInterpolation(float* map, int mapSize, int cellSize, int x0, int y0);

float Noise_CosineInterpolation(int x0, float y0, int x1, float y1, float x2);
void Noise_BicosineInterpolation(float* map, int mapSize, int cellSize, int x0, int y0);

// Height map generators
void Noise_GenerateOctave(float* map, int mapSize, int cellSize, float amplitude, unsigned int seed, enum PerlinInterpolation interpolation);
float* Noise_Generate(int mapSize, float amplitude, unsigned int seed, int* status);
int noise2(int x, int y, int seed);
float perlin2d(float x, float y, float freq, int depth, int seed);

// Map modifications
void Noise_Normalize(float* map, int mapSize);
Gradient Noise_GetGradient(float* map, int mapSize, float posX, float posY);
Gradient Noise_GetSecondGradient(float* map, int mapSize, float posX, float posY);
void Noise_Erode(float* map, int mapSize, float intensity, int* status);

#endif