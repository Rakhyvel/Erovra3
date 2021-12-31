#pragma once

#include "./noise.h"
#include "./debug.h"
#include "arraylist.h"
#include "vector.h"
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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
float Noise_LinearInterpolation(int x0, float y0, int x1, float y1, float x2)
{
    return (float)y0 + (float)(x2 - x0) * (float)(y1 - y0) / (float)(x1 - x0);
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
void Noise_BilinearInterpolation(float* map, int mapSize, int cellSize, int x0, int y0)
{
    int x1, y1;
    if ((x1 = x0 + cellSize) >= mapSize) {
        x1 = x0;
    }
    if ((y1 = y0 + cellSize) >= mapSize) {
        y1 = y0;
    }
    for (int y = y0; y < y0 + cellSize; y++) {
        float z0 = Noise_LinearInterpolation(y0, map[x0 + y0 * mapSize], y0 + cellSize, map[x0 + y1 * mapSize], y);
        float z1 = Noise_LinearInterpolation(y0, map[x1 + y0 * mapSize], y0 + cellSize, map[x1 + y1 * mapSize], y);
        for (int x = x0; x < x0 + cellSize; x++) {
            map[x + y * mapSize] = Noise_LinearInterpolation(x0, z0, x0 + cellSize, z1, x);
        }
    }
}

/*
* Performs the taylor series expansion for cosine. Accurate for range {-pi < x < pi}
*/
double fastCos(double x)
{
    if (x > 3.14159265359) {
        x -= 2 * 3.14159265359;
    } else if (x < -3.14159265359) {
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

/*
	Cosine interpolation for x, given (x0, y0) and (x1, y1) */
float Noise_CosineInterpolation(int x0, float y0, int x1, float y1, float x2)
{
    float mu2 = (1 - fastCos((x2 - x0) * M_PI / (x1 - x0))) / 2;
    return y0 * (1 - mu2) + y1 * mu2;
}

/*
	Cosine interpolation for (x2, y2) given a cell starting at (x0, y0) and a map */
void Noise_BicosineInterpolation(float* map, int mapSize, int cellSize, int x0, int y0)
{
    int x1, y1;
    if ((x1 = x0 + cellSize) >= mapSize) {
        x1 = x0;
    }
    if ((y1 = y0 + cellSize) >= mapSize) {
        y1 = y0;
    }
    for (int y = y0; y < y0 + cellSize; y++) {
        float z0 = Noise_CosineInterpolation(y0, map[x0 + y0 * mapSize], y0 + cellSize, map[x0 + y1 * mapSize], y);
        float z1 = Noise_CosineInterpolation(y0, map[x1 + y0 * mapSize], y0 + cellSize, map[x1 + y1 * mapSize], y);
        for (int x = x0; x < x0 + cellSize; x++) {
            map[x + y * mapSize] = Noise_CosineInterpolation(x0, z0, x0 + cellSize, z1, x);
        }
    }
}

/*
y0 - y value of point to left
x1 - x value of first known point
y1 - y value of first known point
x2 - x value of second known point
y2 - y value of second known point
y3 - y value of point to rihgt
x  - x value to interpolate
*/
float Noise_CubicInterpolation(int cellSize, int x1, float x, float y0, float y1, float y2, float y3)
{
    float scale = (float)(x - x1) / (float)cellSize; // Map [x1,x2] to [0,1]
    float a = (-0.5 * y0 + 1.5 * y1 - 1.5 * y2 + 0.5 * y3) * powf(scale, 3);
    float b = (y0 - 2.5 * y1 + 2 * y2 - 0.5 * y3) * powf(scale, 2);
    float c = (-0.5 * y0 + 0.5 * y2) * scale;
    return a + b + c + y1;
}

/*
	Cosine interpolation for (x2, y2) given a cell starting at (x0, y0) and a map */
void Noise_BicubicInterpolation(float map[], int mapSize, int cellSize, int x1, int y1)
{
    int x0 = max(0, x1 - cellSize);
    int y0 = max(0, y1 - cellSize);
    int x2, y2, x3, y3;
    if ((x2 = x1 + cellSize) >= mapSize) {
        x2 = x1;
    }
    if ((y2 = y1 + cellSize) >= mapSize) {
        y2 = y1;
    }
    if ((x3 = x2 + cellSize) >= mapSize) {
        x3 = x2;
    }
    if ((y3 = y2 + cellSize) >= mapSize) {
        y3 = y2;
    }
    for (int y = y1; y < y1 + cellSize; y++) {
        float z0 = Noise_CubicInterpolation(cellSize, y1, y, map[x0 + y0 * mapSize], map[x0 + y1 * mapSize], map[x0 + y2 * mapSize], map[x0 + y3 * mapSize]);
        float z1 = Noise_CubicInterpolation(cellSize, y1, y, map[x1 + y0 * mapSize], map[x1 + y1 * mapSize], map[x1 + y2 * mapSize], map[x1 + y3 * mapSize]);
        float z2 = Noise_CubicInterpolation(cellSize, y1, y, map[x2 + y0 * mapSize], map[x2 + y1 * mapSize], map[x2 + y2 * mapSize], map[x2 + y3 * mapSize]);
        float z3 = Noise_CubicInterpolation(cellSize, y1, y, map[x3 + y0 * mapSize], map[x3 + y1 * mapSize], map[x3 + y2 * mapSize], map[x3 + y3 * mapSize]);
        for (int x = x1; x < x1 + cellSize; x++) {
            map[x + y * mapSize] = Noise_CubicInterpolation(cellSize, x1, x, z0, z1, z2, z3);
        }
    }
}

/*
Generates a random height map of a given size, that has cells of a given size, and a given maximum/minimum amplitude

Parameters: int mapSize - Height and width of map (maps are squares)
			int cellSize - Height and width of cells. Ought to be a factor of mapSize.
			float amplitude - Max/min amplitude of values in map

Returns: A pointer to the begining of the map. Map is row major ordered as an array of floats of size mapSize * mapSize.
		 Callee is responsible for freeing map.
*/
void Noise_GenerateOctave(float* map, int mapSize, int cellSize, float amplitude, unsigned int seed, enum PerlinInterpolation interpolation)
{
    // Fill lattice with random points
    for (int y = 0; y < mapSize; y += cellSize) {
        for (int x = 0; x < mapSize; x += cellSize) {
            map[y * mapSize + x] = amplitude * ((float)rand()) / ((float)RAND_MAX);
        }
    }

    // Interpolate the rest of the map
    for (int y = 0; y < mapSize; y += cellSize) {
        for (int x = 0; x < mapSize; x += cellSize) {
            switch (interpolation) {
            case BILINEAR:
                Noise_BilinearInterpolation(map, mapSize, cellSize, x, y);
                break;
            case BICOSINE:
                Noise_BicosineInterpolation(map, mapSize, cellSize, x, y);
                break;
            case BICUBIC:
                Noise_BicubicInterpolation(map, mapSize, cellSize, x, y);
                break;
            }
        }
    }
}

static int hash[] = { 208, 34, 231, 213, 32, 248, 233, 56, 161, 78, 24, 140, 71, 48, 140, 254, 245, 255, 247, 247, 40,
    185, 248, 251, 245, 28, 124, 204, 204, 76, 36, 1, 107, 28, 234, 163, 202, 224, 245, 128, 167, 204,
    9, 92, 217, 54, 239, 174, 173, 102, 193, 189, 190, 121, 100, 108, 167, 44, 43, 77, 180, 204, 8, 81,
    70, 223, 11, 38, 24, 254, 210, 210, 177, 32, 81, 195, 243, 125, 8, 169, 112, 32, 97, 53, 195, 13,
    203, 9, 47, 104, 125, 117, 114, 124, 165, 203, 181, 235, 193, 206, 70, 180, 174, 0, 167, 181, 41,
    164, 30, 116, 127, 198, 245, 146, 87, 224, 149, 206, 57, 4, 192, 210, 65, 210, 129, 240, 178, 105,
    228, 108, 245, 148, 140, 40, 35, 195, 38, 58, 65, 207, 215, 253, 65, 85, 208, 76, 62, 3, 237, 55, 89,
    232, 50, 217, 64, 244, 157, 199, 121, 252, 90, 17, 212, 203, 149, 152, 140, 187, 234, 177, 73, 174,
    193, 100, 192, 143, 97, 53, 145, 135, 19, 103, 13, 90, 135, 151, 199, 91, 239, 247, 33, 39, 145,
    101, 120, 99, 3, 186, 86, 99, 41, 237, 203, 111, 79, 220, 135, 158, 42, 30, 154, 120, 67, 87, 167,
    135, 176, 183, 191, 253, 115, 184, 21, 233, 58, 129, 233, 142, 39, 128, 211, 118, 137, 139, 255,
    114, 20, 218, 113, 154, 27, 127, 246, 250, 1, 8, 198, 250, 209, 92, 222, 173, 21, 88, 102, 219 };

int noise2(int x, int y, int seed)
{
    int tmp = hash[(y + seed) % 256];
    return hash[(tmp + x) % 256];
}

float lin_inter(float x, float y, float s)
{
    return x + s * (y - x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3 - 2 * s));
}

float noise2d(float x, float y, int seed)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int, seed);
    int t = noise2(x_int + 1, y_int, seed);
    int u = noise2(x_int, y_int + 1, seed);
    int v = noise2(x_int + 1, y_int + 1, seed);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}

float perlin2d(float x, float y, float freq, int depth, int seed)
{
    float xa = x * freq;
    float ya = y * freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for (i = 0; i < depth; i++) {
        div += 256 * amp;
        fin += noise2d(xa, ya, seed) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin / div;
}

/**
Generates a perlin noise map. Map has fractal detail, so detail on higher
scales and detail on lower scales. This makes it good for terrain

Creates a base noise map at a high amplitude, and low frequency. Layers on more
and more maps with decresing amplitude and increasing frequency.

Parameters: int mapSize - Height and width of the map (maps are squares)
			int cellSize - Size of cells. Lower values = lower grain, higher values = higher grain

Returns: a pointer to a float array, with size of mapSize * mapSize, in row major order.
*/
float* Noise_Generate(int mapSize, int cellSize, unsigned int seed, int* status)
{
    float amplitude = 0.5f;
    float* retval = (float*)malloc(mapSize * mapSize * sizeof(float));
    if (!retval) {
        PANIC("Memory error");
    }

    for (int y = 0; y < mapSize; y++) {
        for (int x = 0; x < mapSize; x++) {
            retval[x + y * mapSize] = perlin2d(x, y, cellSize * 2.0f / mapSize, 10, seed)
                - 0.00f * sqrtf(powf(x - mapSize / 2, 2) + powf(y - mapSize / 2, 2));
        }
        (*status)++;
    }
    return retval; /*

    Noise_GenerateOctave(retval, mapSize, cellSize, amplitude, seed, BICUBIC);
    float* map = (float*)malloc(mapSize * mapSize * sizeof(float));
    if (!map) {
        PANIC("Memory error");
    }
    cellSize /= 2;
    amplitude /= 2;
    int j = 1;

    while (cellSize >= 1) {
        Noise_GenerateOctave(map, mapSize, cellSize, amplitude, seed, BICUBIC);
        if (!map) {
            PANIC("Memory error");
        }
        for (int i = 0; i < mapSize * mapSize; i++) {
            retval[i] += map[i];
        }
        (*status)++;

        cellSize /= 2;
        amplitude *= 0.5f;
        j++;
    }

    free(map);
    return retval;
	*/
}

/*
Normalizes a map so that the lowest value is 0, highest value is 1, and the average value is 0.5

Parameters: float* map - map to normalize
			int mapSize - height and width of map (maps are squares)
*/
void Noise_Normalize(float* map, int mapSize)
{
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

/*
	Finds the gradient on the map at a given point */
Gradient Noise_GetGradient(float* map, int mapSize, float posX, float posY)
{
    if (posX + 1 == mapSize || posY + 1 == mapSize) {
        return (Gradient) { 0, 0 };
    }
    int nodeX = (int)posX;
    int nodeY = (int)posY;

    float x = posX - nodeX;
    float y = posY - nodeY;

    int nodeIndexNW = nodeY * mapSize + nodeX;
    float heightNW = map[nodeIndexNW];
    float heightNE = map[nodeIndexNW + 1];
    float heightSW = map[nodeIndexNW + mapSize];
    float heightSE = map[nodeIndexNW + mapSize + 1];

    // Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
    float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
    float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

    // Calculate height with bilinear interpolation of the heights of the nodes of the cell
    float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
    Gradient grad = { gradientX, gradientY, height };

    return grad;
}

Gradient Noise_GetSecondGradient(float* map, int mapSize, float posX, float posY)
{
    int offset = mapSize / 512;
    if (posX + offset >= mapSize || posY + offset >= mapSize) {
        return (Gradient) { 0, 0 };
    }
    int nodeX = (int)(posX);
    int nodeY = (int)(posY);

    float x = posX - nodeX;
    float y = posY - nodeY;

    int nodeIndexNW = nodeY * mapSize + nodeX;
    float heightNW = map[nodeIndexNW];
    float heightNE = map[nodeIndexNW + offset];
    float heightSW = map[nodeIndexNW + mapSize];
    float heightSE = map[nodeIndexNW + mapSize + offset];

    // Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
    float gradientX = (heightNE - heightNW) * (offset - y) + (heightSE - heightSW) * y;
    float gradientY = (heightSW - heightNW) * (offset - x) + (heightSE - heightNE) * x;

    // Calculate height with bilinear interpolation of the heights of the nodes of the cell
    float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
    Gradient grad = { gradientX, gradientY, height };

    return grad;
}

// Erodes away some terrain
void Noise_Erode(float* map, int mapSize, float intensity, int* status)
{
    srand(0);
    float inertia = 0.1f; // higher/medium values produce smoother maps
    float sedimentCapacityFactor = 400;
    float minSedimentCapacity = 10.0f; // Small values := more deposit
    float depositSpeed = 0.5f;
    float erodeSpeed = 0.5f;
    float evaporateSpeed = 100.0f;
    float gravity = 4.0f;

    for (int i = 0; i < mapSize * mapSize * intensity; i++) {
        float posX = ((float)rand() / (float)RAND_MAX) * (mapSize - 2);
        float posY = ((float)rand() / (float)RAND_MAX) * (mapSize - 2);
        float initPosX = posX;
        float initPosY = posY;
        float dirX = 0;
        float dirY = 0;
        float speed = 1;
        float water = 0;
        float sediment = 0;
        for (int j = 0; j < mapSize / 8.0f; j++) {
            int nodeX = (int)posX;
            int nodeY = (int)posY;
            inertia = map[nodeX + nodeY * mapSize];
            int dropletIndex = nodeX + nodeY * mapSize;
            float cellOffsetX = posX - nodeX;
            float cellOffsetY = posY - nodeY;

            Gradient grad = Noise_GetGradient(map, mapSize, posX, posY);
            // Update the droplet's movement
            dirX = (dirX * inertia - grad.gradX * (1 - inertia));
            dirY = (dirY * inertia - grad.gradY * (1 - inertia));
            // Normalize direction
            float len = sqrtf(dirX * dirX + dirY * dirY);
            if (len != 0) {
                dirX /= len;
                dirY /= len;
            }
            posX += dirX;
            posY += dirY;

            // Stop simulating droplet if it's not moving or has flowed over edge of map
            if ((dirX == 0 && dirY == 0) || posX <= 0 || posX >= mapSize - 1 || posY <= 0 || posY >= mapSize - 1) {
                break;
            }

            // Find the droplet's new height and calculate the deltaHeight
            float newHeight = Noise_GetGradient(map, mapSize, posX, posY).z;
            float deltaHeight = newHeight - grad.z;

            // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
            float sedimentCapacity = max(-deltaHeight * speed * water * sedimentCapacityFactor, minSedimentCapacity);

            float deltaZ;
            // If carrying more sediment than capacity, or if flowing uphill:
            if (sediment > sedimentCapacity || deltaHeight > 0) {
                deltaZ = (deltaHeight > 0) ? min(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
            } else {
                deltaZ = -min((sedimentCapacity - sediment) * erodeSpeed, (float)fabs(deltaHeight));
            }
            sediment -= deltaZ;
            map[dropletIndex] += deltaZ * (1 - cellOffsetX) * (1 - cellOffsetY);
            map[dropletIndex + 1] += deltaZ * cellOffsetX * (1 - cellOffsetY);
            map[dropletIndex + mapSize] += deltaZ * (1 - cellOffsetX) * cellOffsetY;
            map[dropletIndex + mapSize + 1] += deltaZ * cellOffsetX * cellOffsetY;

            // Update droplet's speed and water content
            speed = sqrtf(speed * speed + deltaHeight * gravity);
            water *= (1 - evaporateSpeed);
        }
        (*status)++;
    }
}

bool Noise_IsBorder(float* terrain, int width, int height, int x, int y, float z, int i)
{
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
