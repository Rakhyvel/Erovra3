#pragma once

#include "./perlin.h"
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
float Perlin_LinearInterpolation(int x0, float y0, int x1, float y1, float x2)
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
float Perlin_BilinearInterpolation(float* map, int mapSize, int cellSize, int x0, int y0, float x2, float y2)
{
    int x1 = x0 + cellSize;
    int y1 = y0 + cellSize;
    if (x1 >= mapSize) {
        x1 = 0;
    }
    if (y1 >= mapSize) {
        y1 = 0;
    }

    float top = Perlin_LinearInterpolation(x0, map[y0 * mapSize + x0], x0 + cellSize, map[y0 * mapSize + x1], x2);
    float bottom = Perlin_LinearInterpolation(x0, map[y1 * mapSize + x0], x0 + cellSize, map[y1 * mapSize + x1], x2);

    return Perlin_LinearInterpolation(y0, top, y0 + cellSize, bottom, y2);
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
float Perlin_CosineInterpolation(int x0, float y0, int x1, float y1, float x)
{
    double xDiff = (double)(x1 - x0);
    double mu2 = (1 - fastCos((double)(x - x0) * 3.141592653589793 / xDiff)) / 2;
    double retval = (y0 * (1 - mu2) + y1 * mu2);
    return (float)retval;
}

/*
	Cosine interpolation for (x2, y2) given a cell starting at (x0, y0) and a map */
float Perlin_BicosineInterpolation(float* map, int mapSize, int cellSize, int x0, int y0, float x2, float y2)
{
    int x1 = x0 + cellSize;
    int y1 = y0 + cellSize;
    if (x1 >= mapSize) {
        x1 -= cellSize;
    }
    if (y1 >= mapSize) {
        y1 -= cellSize;
    }

    float top = Perlin_CosineInterpolation(x0, map[y0 * mapSize + x0], x0 + cellSize, map[(int)(y0 * mapSize + x1)], x2);
    float bottom = Perlin_CosineInterpolation(x0, map[y1 * mapSize + x0], x0 + cellSize, map[(int)(y1 * mapSize + x1)], x2);

    return Perlin_CosineInterpolation(y0, top, y0 + cellSize, bottom, y2);
}

/*
Generates a random height map of a given size, that has cells of a given size, and a given maximum/minimum amplitude

Parameters: int mapSize - Height and width of map (maps are squares)
			int cellSize - Height and width of cells. Ought to be a factor of mapSize.
			float amplitude - Max/min amplitude of values in map

Returns: A pointer to the begining of the map. Map is row major ordered as an array of floats of size mapSize * mapSize.
		 Callee is responsible for freeing map.
*/
void Perlin_GenerateOctave(float* map, int mapSize, int cellSize, float amplitude, unsigned int seed, enum PerlinInterpolation interpolation)
{
    srand(seed);

    // For each node assign it a height
    for (int y = 0; y < mapSize; y += cellSize) {
        for (int x = 0; x < mapSize; x += cellSize) {
            float r = ((float)rand()) / ((float)RAND_MAX);
            r = cosf(r);
            map[y * mapSize + x] = amplitude * r;
        }
    }

    // Interpolate the rest of the map
    for (int i = 0; i < mapSize * mapSize; i++) {
        int x = i % mapSize; // x coord of point
        int y = i / mapSize; // y coord of point
        int x1 = (int)((x / cellSize) * cellSize); // x coord of point's cell
        int y1 = (int)((y / cellSize) * cellSize); // y coord of point's cell

        if (x != x1 || y != y1) {
            switch (interpolation) {
            case BILINEAR:
                map[x + y * mapSize] = Perlin_BilinearInterpolation(map, mapSize, cellSize, x1, y1, (float)x, (float)y);
                break;
            case BICOSINE:
                map[x + y * mapSize] = Perlin_BicosineInterpolation(map, mapSize, cellSize, x1, y1, (float)x, (float)y);
                break;
            }
        }
    }
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
float* Perlin_Generate(int mapSize, int cellSize, unsigned int seed, int* status)
{
    float amplitude = 0.5f;
    float* retval = (float*)malloc(mapSize * mapSize * sizeof(float));
    if (!retval) {
        PANIC("Memory error");
    }
    Perlin_GenerateOctave(retval, mapSize, cellSize, amplitude, seed, BICOSINE);
    float* map = (float*)malloc(mapSize * mapSize * sizeof(float));
    if (!map) {
        PANIC("Memory error");
    }
    cellSize /= 2;
    amplitude /= 2;
    int j = 1;

    while (cellSize >= 1) {
        Perlin_GenerateOctave(map, mapSize, cellSize, amplitude, seed, BICOSINE);
        if (!map) {
            PANIC("Memory error");
        }
        for (int i = 0; i < mapSize * mapSize; i++) {
            retval[i] += map[i];
        }
        (*status)++;
        printf("%d\n", *status);

        cellSize /= 2;
        amplitude *= 0.5f;
        j++;
    }
    
    Perlin_GenerateOctave(map, mapSize, mapSize / 4, 1, seed, BICOSINE);
    for (int i = 0; i < mapSize * mapSize; i++) {
        retval[i] = retval[i] * 0.75 + map[i] * 0.25;
    }
    free(map);
    return retval;
}

/*
Normalizes a map so that the lowest value is 0, highest value is 1, and the average value is 0.5

Parameters: float* map - map to normalize
			int mapSize - height and width of map (maps are squares)
*/
void Perlin_Normalize(float* map, int mapSize)
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
Gradient Perlin_GetGradient(float* map, int mapSize, float posX, float posY)
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

Gradient Perlin_GetSecondGradient(float* map, int mapSize, float posX, float posY)
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
void Perlin_Erode(float* map, int mapSize, float intensity, int* status)
{
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

            Gradient grad = Perlin_GetGradient(map, mapSize, posX, posY);
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
            float newHeight = Perlin_GetGradient(map, mapSize, posX, posY).z;
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

bool Perlin_IsBorder(float* terrain, int width, int height, int x, int y, float z, int i)
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
