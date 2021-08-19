/*
terrain.c
*/

#pragma once
#include <SDL.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./engine/gameState.h"
#include "./util/debug.h"
#include "./util/vector.h"
#include "terrain.h"

static bool mouseDown = false;
static struct vector offset = { 0, 0 };
static struct vector oldOffset = { 0, 0 };
static struct vector mousePos = { 0, 0 };
static float terrain_zoom_target = 1;
static float terrain_zoom = 0.8;
static int oldWheel = 0;

/*
	Creates the terrain struct, with the height map, ore map, and terrain 
	texture. */
struct terrain* terrain_create(int mapSize, float* map, SDL_Texture* texture)
{
    struct terrain* retval = calloc(1, sizeof(struct terrain));
    if (!retval) {
        fprintf(stderr, "Memory error terrain_create() creating the terrain\n");
        exit(1);
    }
    retval->size = mapSize;
    retval->tileSize = mapSize / 64;
    retval->map = map;
    retval->texture = texture;

    int status = 0;
    retval->ore = terrain_perlin(retval->tileSize, retval->tileSize / 8, 0, &status);
    retval->buildings = (EntityID*)malloc(retval->tileSize * retval->tileSize * sizeof(EntityID));
    if (!retval->buildings) {
        PANIC("Memory error");
        exit(1);
    }
    for (int i = 0; i < (retval->tileSize * retval->tileSize); i++) {
        retval->buildings[i] = INVALID_ENTITY_INDEX;
    }
    retval->walls = (EntityID*)malloc(((mapSize + 1) * (mapSize + 1)) / (32 * 32) * sizeof(EntityID));
    if (!retval->walls) {
        PANIC("Memory error");
        exit(1);
    }
    for (int i = 0; i < ((mapSize + 1) * (mapSize + 1)) / (32 * 32); i++) {
        retval->walls[i] = INVALID_ENTITY_INDEX;
    }
    terrain_normalize(retval->ore, retval->tileSize);
    return retval;
}

/*
	Takes in a terrain struct, initializes the texture for it, and sets the pixels
	based on the heightmap and a color function */
void paintMap(int size, float* map, SDL_Texture* texture)
{
    Uint8* pixels = malloc(max(16, size * size) * 4);
    if (!pixels) {
        fprintf(stderr, "Memory error paintMap() creating pixels\n");
        exit(1);
    }
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            const unsigned int offset = (size * 4 * y) + x * 4;
            SDL_Color terrainColor;
            float i = map[y * size + x];
            i = i * 2 - 0.5f;
            float g = 0;
            if (x < size - 2 && y < size - 2) {
                Gradient grad = terrain_getGradient(size, map, x, y);
                g = sqrtf(grad.gradX * grad.gradX + grad.gradY * grad.gradY);
            }
            if (i < 0.5) {
                // water
                i *= 2;
                i = powf(i, 9);
                terrainColor = terrain_HSVtoRGB(214.0f - 25.0f * i, 0.92f - 0.45f * i, 0.21f + 0.6f * i + 0.1 * powf(i, 91));
            } else {
                // ground
                i = (i - 0.5f) * 2;
                i = sqrtf(i);
                i = sqrtf(i);
                terrainColor = terrain_HSVtoRGB(27.0f + 85.0f * i, (i * 0.2f) + 0.2f, 0.96f - i * 0.6f);
            }
            if (size >= 256.0f && terrain_isBorder(map, size, size, x, y, 0.5f, 1)) {
                terrainColor = (SDL_Color) { 255, 255, 255 };
            }

            pixels[offset + 0] = terrainColor.b;
            pixels[offset + 1] = terrainColor.g;
            pixels[offset + 2] = terrainColor.r;
            pixels[offset + 3] = SDL_ALPHA_OPAQUE;
        }
    }
    if (SDL_UpdateTexture(texture, NULL, pixels, size * 4) == -1) {
        PANIC("%s", SDL_GetError());
    }
    free(pixels);
}

/*
	Sets the terrain offset and zoom depending on the game state's input */
void terrain_update(struct terrain* terrain)
{
    terrain_zoom *= (float)pow(1.1, g->mouseWheelY - oldWheel);
    oldWheel = g->mouseWheelY;
    if (terrain_zoom < 0.75 * g->height / terrain->size) {
        terrain_zoom = 0.75f * g->height / terrain->size;
    }
    if (terrain_zoom > 8.0f * g->height / terrain->size) {
        terrain_zoom = 8.0f * g->height / terrain->size;
    }

    if (g->mouseLeftDown && !mouseDown) {
        mouseDown = true;
    }
    if (!g->mouseLeftDown && mouseDown) {
        mouseDown = false;
        oldOffset.x = offset.x;
        oldOffset.y = offset.y;
    }
    if (g->mouseMoved) {
        mousePos.x = (float)(((float)g->mouseX - (float)g->width / 2.0f) / terrain_zoom) - offset.x;
        mousePos.y = (float)(((float)g->mouseY - (float)g->height / 2.0f) / terrain_zoom) - offset.y;
    }
    if (g->mouseDrag && !g->shift) {
        offset.x = (g->mouseX - g->mouseInitX) / terrain_zoom + oldOffset.x;
        offset.y = (g->mouseY - g->mouseInitY) / terrain_zoom + oldOffset.y;
    }
}

/*
	Renders the terrain's texture to the screen, and some grid lines */
void terrain_render(struct terrain* terrain)
{
    SDL_Rect rect = { 0, 0, 0, 0 };
    terrain_translate(&rect, 0, 0, 0, 0);
    rect.w = rect.h = (int)(terrain_zoom * terrain->size);
    SDL_RenderCopy(g->rend, terrain->texture, NULL, &rect);

    SDL_SetRenderDrawColor(g->rend, 0, 0, 0, 50);
    SDL_FPoint gridLineStart = { 32, 32 };
    SDL_Rect gridLineRect = { 0, 0, 0, 0 };
    terrain_translate(&gridLineRect, gridLineStart.x, gridLineStart.y, 64, 64);
    for (int x = 0; x <= terrain->tileSize; x++) {
        SDL_RenderDrawLine(g->rend,
            max((int)(gridLineRect.x + x * 64.0 * terrain_zoom), 0),
            gridLineRect.y,
            (int)(gridLineRect.x + x * 64.0 * terrain_zoom),
            (int)(gridLineRect.y + (terrain->size * terrain_zoom)));
    }
    for (int y = 0; y <= terrain->tileSize; y++) {
        SDL_RenderDrawLine(g->rend,
            max(gridLineRect.x, 0),
            (int)(gridLineRect.y + y * 64.0 * terrain_zoom),
            (int)(gridLineRect.x + (terrain->size * terrain_zoom)),
            (int)(gridLineRect.y + y * 64.0 * terrain_zoom));
    }

    SDL_RenderDrawRect(g->rend, &rect);
    rect.x += 1;
    rect.y += 1;
    rect.w -= 2;
    rect.h -= 2;
    SDL_RenderDrawRect(g->rend, &rect);
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
float terrain_linearInterpolation(int x0, float y0, int x1, float y1, float x2)
{
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
float terrain_bilinearInterpolation(int x0, int y0, int cellSize, float* map, int mapWidth, float x2, float y2)
{
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
float terrain_cosineInterpolation(int x0, float y0, int x1, float y1, float x)
{
    double xDiff = (double)(x1 - x0);
    double mu2 = (1 - fastCos((double)(x - x0) * 3.141592653589793 / xDiff)) / 2;
    double retval = (y0 * (1 - mu2) + y1 * mu2);
    return (float)retval;
}

/*
	Cosine interpolation for (x2, y2) given a cell starting at (x0, y0) and a map */
float terrain_bicosineInterpolation(int x0, int y0, int cellSize, float* map, int mapWidth, float x2, float y2)
{
    int x1 = x0 + cellSize;
    int y1 = y0 + cellSize;
    if (x1 >= mapWidth) {
        x1 -= cellSize;
    }
    if (y1 >= mapWidth) {
        y1 -= cellSize;
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
float* terrain_generate(int mapSize, int cellSize, float amplitude, unsigned int seed)
{
    srand(seed);
    // Allocate map
    float* retval = (float*)malloc(mapSize * mapSize * sizeof(float));
    if (!retval) {
        PANIC("Memory error");
    }

    int n = (mapSize * mapSize) / (cellSize * cellSize);
    bool* used = (bool*)calloc(n, sizeof(bool));
    float* values = (float*)malloc(n * sizeof(float));
    // Setting up random values from [0-amplitude)
    for (int i = 0; i < n; i++) {
        values[i] = (float)i / (float)n * amplitude;
    }

    // For each node assign it a height
    for (int y = 0; y < mapSize; y += cellSize) {
        for (int x = 0; x < mapSize; x += cellSize) {
            // Randomly choose an index
            int randIndex = rand() % n;
            // If the index is already used, find next available index
            while (used[randIndex]) {
                randIndex++;
                // Wrap around
                if (randIndex >= n) {
                    randIndex = 0;
                }
            }
            retval[y * mapSize + x] = values[randIndex];
            used[randIndex] = true;
        }
    }
    free(used);
    free(values);
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

Creates a base noise map at a high amplitude, and low frequency. Layers on more
and more maps with decresing amplitude and increasing frequency.

Parameters: int mapSize - Height and width of the map (maps are squares)
			int cellSize - Size of cells. Lower values = lower grain, higher values = higher grain

Returns: a pointer to a float array, with size of mapSize * mapSize, in row major order.
*/
float* terrain_perlin(int mapSize, int cellSize, unsigned int seed, int* status)
{
    float amplitude = 0.5;
    float* retval = terrain_generate(mapSize, cellSize, amplitude, seed);
    cellSize /= 2;
    amplitude /= 2;

    while (mapSize > 64 * 8 ? cellSize > 8 : cellSize > 1) {
        float* map = terrain_generate(mapSize, cellSize, amplitude, seed);
        for (int i = 0; i < mapSize * mapSize; i++) {
            retval[i] += map[i];
        }
        free(map);
        (*status)++;

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
void terrain_normalize(float* map, int mapSize)
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
Gradient terrain_getGradient(int size, float* map, float posX, float posY)
{
    int nodeX = (int)posX;
    int nodeY = (int)posY;

    float x = posX - nodeX;
    float y = posY - nodeY;

    int nodeIndexNW = nodeY * size + nodeX;
    float heightNW = map[nodeIndexNW];
    float heightNE = map[nodeIndexNW + 1];
    float heightSW = map[nodeIndexNW + size];
    float heightSE = map[nodeIndexNW + size + 1];

    // Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
    float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
    float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

    // Calculate height with bilinear interpolation of the heights of the nodes of the cell
    float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
    Gradient grad = { gradientX, gradientY, height };

    return grad;
}

// Intensity should range from 0 to 3
void terrain_erode(int size, float* map, float intensity, int* status)
{
    float inertia = 0.05f; // higher/medium values produce smoother maps
    float sedimentCapacityFactor = 400;
    float minSedimentCapacity = 0.01f; // Small values := more deposit
    float depositSpeed = 0.3f;
    float erodeSpeed = 0.3f;
    float evaporateSpeed = 100.0f;
    float gravity = 4;

    int* erosionBrushIndices = NULL;
    float* erosionBrushWeights = NULL;
    for (int i = 0; i < size * size * intensity; i++) {
        // Create random droplet
        float posX = ((float)rand() / (float)RAND_MAX) * (size - 2);
        float posY = ((float)rand() / (float)RAND_MAX) * (size - 2);
        float dirX = 0;
        float dirY = 0;
        float speed = 1;
        float water = 0;
        float sediment = 0;
        for (int j = 0; j < size / 64.0f * 3; j++) {
            int nodeX = (int)posX;
            int nodeY = (int)posY;
            int dropletIndex = nodeX + nodeY * size;
            float cellOffsetX = posX - nodeX;
            float cellOffsetY = posY - nodeY;

            Gradient grad = terrain_getGradient(size, map, posX, posY);

            // Update the droplet's movement
            dirX = (dirX * inertia - grad.gradX * (1 - inertia));
            dirY = (dirY * inertia - grad.gradY * (1 - inertia));
            // Normalize direction
            float len = sqrtf(dirX * dirX + dirY * dirY) * 0.9;
            if (len != 0) {
                dirX /= len;
                dirY /= len;
            }
            posX += dirX;
            posY += dirY;

            // Stop simulating droplet if it's not moving or has flowed over edge of map
            if ((dirX == 0 && dirY == 0) || posX < 0 || posX >= size - 1 || posY < 0 || posY >= size - 1) {
                break;
            }

            // Find the droplet's new height and calculate the deltaHeight
            float newHeight = terrain_getGradient(size, map, posX, posY).z;
            float deltaHeight = newHeight - grad.z;

            // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
            float sedimentCapacity = max(-deltaHeight * speed * water * sedimentCapacityFactor, minSedimentCapacity);

            // If carrying more sediment than capacity, or if flowing uphill:
            if (sediment > sedimentCapacity || deltaHeight > 0) {
                // If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
                float amountToDeposit = (deltaHeight > 0) ? min(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
                sediment -= amountToDeposit;

                // Add the sediment to the four nodes of the current cell using bilinear interpolation
                // Deposition is not distributed over a radius (like erosion) so that it can fill small pits
                map[dropletIndex] += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetY);
                map[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1 - cellOffsetY);
                map[dropletIndex + size] += amountToDeposit * (1 - cellOffsetX) * cellOffsetY;
                map[dropletIndex + size + 1] += amountToDeposit * cellOffsetX * cellOffsetY;
            } else {
                // Erode a fraction of the droplet's current carry capacity.
                // Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
                float amountToErode = min((sedimentCapacity - sediment) * erodeSpeed, fabs(deltaHeight));
                sediment += amountToErode;

                // Use erosion brush to erode from all nodes inside the droplet's erosion radius
                map[dropletIndex] -= amountToErode * (1 - cellOffsetX) * (1 - cellOffsetY);
                map[dropletIndex + 1] -= amountToErode * cellOffsetX * (1 - cellOffsetY);
                map[dropletIndex + size] -= amountToErode * (1 - cellOffsetX) * cellOffsetY;
                map[dropletIndex + size + 1] -= amountToErode * cellOffsetX * cellOffsetY;
            }

            // Update droplet's speed and water content
            speed = sqrtf(speed * speed + deltaHeight * gravity);
            water *= (1 - evaporateSpeed);
        }
        (*status)++;
    }
}

/*
	Returns the height at a given point (x, y). If point is outside bounds, 
	returns -1 */
float terrain_getHeight(struct terrain* terrain, int x, int y)
{
    if (x < 0 || y < 0 || x >= terrain->size || y >= terrain->size) {
        return -1;
    }
    return terrain->map[y * terrain->size + x];
}

/*
	Sets the offset */
void terrain_setOffset(struct vector vector)
{
    offset.x = -vector.x;
    offset.y = -vector.y;
    oldOffset.x = offset.x;
    oldOffset.y = offset.y;
}

inline float terrain_getZoom()
{
    return terrain_zoom;
}

float terrain_getOre(struct terrain* terrain, int x, int y)
{
    x /= 64;
    y /= 64;
    return terrain->ore[x + y * (terrain->tileSize)];
}

float terrain_getHeightForBuilding(struct terrain* terrain, int x, int y)
{
    return terrain_getHeight(terrain, 64 * (int)(x / 64) + 32, 64 * (int)(y / 64) + 32);
}

EntityID terrain_getBuildingAt(struct terrain* terrain, int x, int y)
{
    x -= 32;
    y -= 32;
    x /= 64;
    y /= 64;
    return terrain->buildings[x + y * (terrain->tileSize)];
}

void terrain_setBuildingAt(struct terrain* terrain, EntityID id, int x, int y)
{
    x -= 32;
    y -= 32;
    x /= 64;
    y /= 64;
    printf("%d\n", (id >> 16) & 0xFFFF);
    terrain->buildings[x + y * (terrain->tileSize)] = id;
}

EntityID terrain_getWallAt(struct terrain* terrain, int x, int y)
{
    x /= 32;
    y /= 32;
    return terrain->walls[x + y * (terrain->tileSize)];
}

void terrain_setWallAt(struct terrain* terrain, EntityID id, int x, int y)
{
    x /= 32;
    y /= 32;
    terrain->walls[x + y * (terrain->tileSize)] = id;
}

int terrain_closestBuildingDist(struct terrain* terrain, int x1, int y1)
{
    x1 -= 32;
    y1 -= 32;
    x1 /= 64;
    y1 /= 64;
    int retval = terrain->size * 2;
    for (int x = 0; x < terrain->tileSize; x++) {
        for (int y = 0; y < terrain->tileSize; y++) {
            if (terrain->buildings[x + y * terrain->tileSize] != INVALID_ENTITY_INDEX) {
                int dist = abs(x1 - x) + abs(y1 - y);
                if (dist < retval) {
                    retval = dist;
                }
            }
        }
    }
    return retval;
}

int terrain_closestMaskDist(struct scene* scene, ComponentMask mask, struct terrain* terrain, int x1, int y1)
{
    x1 -= 32;
    y1 -= 32;
    x1 /= 64;
    y1 /= 64;
    int retval = terrain->size * 2;
    for (int x = 0; x < terrain->tileSize; x++) {
        for (int y = 0; y < terrain->tileSize; y++) {
            EntityID buildingID = terrain->buildings[x + y * terrain->tileSize];
            if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponent(scene, mask, buildingID)) {
                int dist = abs(x1 - x) + abs(y1 - y);
                if (dist < retval) {
                    retval = dist;
                }
            }
        }
    }
    return retval;
}

/*
	Checks the four adjacent tiles to see if there is a building with the 
	entity mask given. If so, returns that entity ID. If not, returns 
	INVALID_ENTITY_INDEX */
EntityID terrain_adjacentMask(struct scene* scene, ComponentMask mask, struct terrain* terrain, int x, int y)
{
    x /= 64;
    y /= 64;
    if (x > 0) {
        EntityID buildingID = terrain->buildings[(x - 1) + y * terrain->tileSize];
        if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponent(scene, mask, buildingID)) {
            return buildingID;
        }
    }
    if (y > 0) {
        EntityID buildingID = terrain->buildings[x + (y - 1) * terrain->tileSize];
        if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponent(scene, mask, buildingID)) {
            return buildingID;
        }
    }
    if (x < terrain->tileSize) {
        EntityID buildingID = terrain->buildings[(x + 1) + y * terrain->tileSize];
        if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponent(scene, mask, buildingID)) {
            return buildingID;
        }
    }
    if (y < terrain->tileSize) {
        EntityID buildingID = terrain->buildings[x + (y + 1) * terrain->tileSize];
        if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponent(scene, mask, buildingID)) {
            return buildingID;
        }
    }
    return INVALID_ENTITY_INDEX;
}

/*
	Takes in a pointer to an SDL_Rect struct, the position and size of a sprite. 
	Translates and scales the rect based on the offset and zoom 
	
	Converts map coords to screen coords*/
void terrain_translate(SDL_Rect* newPos, float x, float y, float width, float height)
{
    newPos->x = ((x + offset.x - width / 2.0f) * terrain_zoom + g->width / 2.0f);
    newPos->y = ((y + offset.y - height / 2.0f) * terrain_zoom + g->height / 2.0f);
    newPos->w = (width * terrain_zoom);
    newPos->h = (height * terrain_zoom);
}
struct vector Terrain_MousePos()
{
    return mousePos;
}

// Converts screen coords to map coords
SDL_FPoint terrain_inverseTranslate(int x, int y)
{
    SDL_FPoint newPos;
    newPos.x = (x - g->width / 2) / terrain_zoom - offset.x;
    newPos.y = (y - g->height / 2) / terrain_zoom - offset.y;
    return newPos;
}

/*
This checks to see if a given point is a border between two values
*/
bool terrain_isBorder(float* terrain, int width, int height, int x, int y, float z, int i)
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
    } else if (hue >= 60 && hue < 120) {
        r = x;
        g = c;
        b = 0;
    } else if (hue >= 120 && hue < 180) {
        r = 0;
        g = c;
        b = x;
    } else if (hue >= 180 && hue < 240) {
        r = 0;
        g = x;
        b = c;
    } else if (hue >= 240 && hue < 300) {
        r = x;
        g = 0;
        b = c;
    } else if (hue >= 300 && hue < 360) {
        r = c;
        g = 0;
        b = x;
    }
    SDL_Color color = { (Uint8)((r + m) * 255), (Uint8)((g + m) * 255), (Uint8)((b + m) * 255) };
    return color;
}

/*
	Takes two positions, checks to see if there is dry land between them */
bool terrain_lineOfSight(struct terrain* terrain, Vector from, Vector to, float z)
{
    if (to.x < 0 || to.x >= terrain->size || to.y < 0 || to.y >= terrain->size) {
        return false;
    }
    if (z > 0.5) {
        return true;
    }
    Vector increment = Vector_Scalar(Vector_Normalize(Vector_Sub(to, from)), 0.5);
    Vector check = { from.x, from.y };
    float distance = Vector_Dist(from, to);
    for (double i = 0; i < distance; i += 0.5) {
        check.x += increment.x;
        check.y += increment.y;
        float height = terrain_getHeight(terrain, check.x, check.y);
        if (height > z + 0.5f || height < z) {
            return false;
        }
    }
    return true;
}

Vector terrain_lineOfSightPoint(struct terrain* terrain, Vector from, Vector to)
{
    Vector increment = Vector_Normalize(Vector_Sub(to, from));
    Vector check = { from.x, from.y };
    float distance = Vector_Dist(from, to);
    for (int i = 0; i < distance; i += 1) {
        check.x += increment.x;
        check.y += increment.y;
        float height = terrain_getHeight(terrain, check.x, check.y);
        if (height > 1 || height < 0.5) {
            return check;
        }
    }
    return check;
}

bool terrain_isSolidSquare(struct terrain* terrain, Vector point)
{
    // Must be land
    if (!terrain_lineOfSight(terrain, point, Vector_Add(point, (Vector) { 32, 0 }), 0.5))
        return false;
    if (!terrain_lineOfSight(terrain, point, Vector_Add(point, (Vector) { -32, 0 }), 0.5))
        return false;
    if (!terrain_lineOfSight(terrain, point, Vector_Add(point, (Vector) { 0, 32 }), 0.5))
        return false;
    if (!terrain_lineOfSight(terrain, point, Vector_Add(point, (Vector) { 0, -32 }), 0.5))
        return false;
}

/*
	Takes in a seed starting location. Searches the entire map for the location 
	that is closest to the starting point, and is above sea level */
struct vector findBestLocation(struct terrain* terrain, struct vector start)
{
    // Search for closest unvisited tile
    struct vector closestTile = { 0, 0 };
    float tempDist = FLT_MAX;
    for (int y = 0; y < terrain->size; y++) {
        for (int x = 0; x < terrain->size; x++) {
            struct vector point = { x * 64 + 32, y * 64 + 32 };
            // Must be land
            if (!terrain_lineOfSight(terrain, point, Vector_Add(point, (Vector) { 32, 0 }), 0.5))
                continue;
            if (!terrain_lineOfSight(terrain, point, Vector_Add(point, (Vector) { -32, 0 }), 0.5))
                continue;
            if (!terrain_lineOfSight(terrain, point, Vector_Add(point, (Vector) { 0, 32 }), 0.5))
                continue;
            if (!terrain_lineOfSight(terrain, point, Vector_Add(point, (Vector) { 0, -32 }), 0.5))
                continue;
            double score = Vector_Dist(start, point);

            // Must have direct line of sight to tile center
            if (score < tempDist) {
                tempDist = score;
                closestTile = point;
            }
        }
    }

    return closestTile;
}