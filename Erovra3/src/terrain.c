﻿/*
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
#include "./util/perlin.h"
#include "./util/vector.h"
#include "terrain.h"

static bool mouseDown = false;
static struct vector offset = { 0, 0 };
static struct vector oldOffset = { 0, 0 };
static struct vector mousePos = { 0, 0 };
static float terrain_zoom_target = 1;
static float terrain_zoom = 0.8f;
static int oldWheel = 0;

/*
	Creates the terrain struct, with the height map, ore map, and terrain 
	texture. */
struct terrain* Terrain_Create(int mapSize, float* map, SDL_Texture* texture)
{
    struct terrain* retval = calloc(1, sizeof(struct terrain));
    if (!retval) {
        PANIC("Memory error Terrain_Create() creating the terrain\n");
    }
    retval->size = mapSize;
    retval->tileSize = mapSize / 64;
    retval->map = map;
    retval->texture = texture;

    int status = 0;
    retval->ore = Perlin_Generate(retval->tileSize, retval->tileSize / 8, (unsigned)time(0), &status);
    Perlin_Normalize(retval->ore, retval->tileSize);
    for (int i = 0; i < (retval->tileSize * retval->tileSize); i++) {
        int x = (i % retval->tileSize) * 64;
        int y = (i / retval->tileSize) * 64;
        retval->ore[i] *= 0.5f * (retval->map[x + y * mapSize] + 1.0f);
    }
    retval->continents = calloc(retval->size * retval->size, sizeof(int));
    if (!retval->continents) {
        PANIC("Memory error");
    }
    retval->buildings = malloc(retval->tileSize * retval->tileSize * sizeof(EntityID));
    if (!retval->buildings) {
        PANIC("Memory error");
    }
    for (int i = 0; i < (retval->tileSize * retval->tileSize); i++) {
        retval->buildings[i] = INVALID_ENTITY_INDEX;
    }
    retval->walls = (EntityID*)calloc(((mapSize + 1) * (mapSize + 1)) / (32 * 32), sizeof(EntityID));
    if (!retval->walls) {
        PANIC("Memory error");
        exit(1);
    }
    for (int i = 0; i < ((mapSize + 1) * (mapSize + 1)) / (32 * 32); i++) {
        retval->walls[i] = INVALID_ENTITY_INDEX;
    }
    retval->ports = Arraylist_Create(10, sizeof(Vector));
    Terrain_FindPorts(retval);
    Terrain_FindContinents(retval);
    return retval;
}

void Terrain_Destroy(struct terrain* terrain)
{
    // DO NOT free map, that is not generated by terrain, and so is not terrain's to free
    free(terrain->ore);
    free(terrain->continents);
    free(terrain->buildings);
    free(terrain->walls);
    Arraylist_Destroy(terrain->ports);
    SDL_DestroyTexture(terrain->texture);
    free(terrain);
}

static float minGrad = 0;
SDL_Color Terrain_RealisticColor(float* map, int mapSize, int x, int y, float i)
{
    i = i * 2 - 0.5f;

    struct gradient grad = Perlin_GetSecondGradient(map, mapSize, (int)(x / 2) * 2, (int)(y / 2) * 2);
    if (grad.gradY < minGrad) {
        minGrad = grad.gradY;
    }

    if (mapSize >= 512.0f && Terrain_IsBorder(map, mapSize, mapSize, x, y, 0.5f, 1)) {
        return (SDL_Color) { 255, 255, 255 };
    } else if (i < 0.5) {
        // water
        i *= 2;
        i = powf(i, 51);
        return Terrain_HSVtoRGB(214.0f - 25.0f * i, 1.0f - 0.45f * i, 0.21f + 0.6f * i + 0.1f * powf(i, 91));
    } else {
        // ground
        i = (i - 0.5f) * 2;
        i = powf(i, 0.25f);
        float light = 1.0f;
        if (x < mapSize - 1 && y < mapSize - 1) {
            // point a
            int a_x = x;
            int a_y = y;
            float a_z = mapSize / 16 * map[a_x + a_y * mapSize] * i * i * i * i * i;
            // point b
            int b_x = x + 1;
            int b_y = y;
            float b_z = mapSize / 16 * map[b_x + b_y * mapSize] * i * i * i * i * i;
            // point c
            int c_x = x;
            int c_y = y + 1;
            float c_z = mapSize / 16 * map[c_x + c_y * mapSize] * i * i * i * i * i;
            // vector q
            float q_x = a_x - b_x;
            float q_y = a_y - b_y;
            float q_z = a_z - b_z;
            // vector r
            float r_x = a_x - c_x;
            float r_y = a_y - c_y;
            float r_z = a_z - c_z;
            // normal
            float n_x = (q_y * r_z) - (q_z * r_y);
            float n_y = (q_z * r_x) - (q_x * r_z);
            float n_z = (q_x * r_y) - (q_y * r_x);
            // normalize
            float mag = n_x * n_x + n_y * n_y + n_z * n_z;
            if (mag != 0) {
                n_y /= mag;
                n_z /= mag;
            }
            light = max(0.0f, -0.707 * n_y + 0.707 * n_z);
        }
        i *= 100;
        float hue = min(120, 0.0000000369 * powf(i, 5) - 0.00000714407361075 * powf(i, 4) + 0.000381833965482 * powf(i, 3) - 0.00120026434352 * powf(i, 2) + 0.207995982732 * i + 50.3635585447);
        float saturation = 0.01 * (0.00000104466482419f * powf(hue, 5) - 0.000370237098314f * powf(hue, 4) + 0.0514055142232 * powf(hue, 3) - 3.4855548673f * powf(hue, 2) + 115.271785291 * hue - 1464.19348868);
        float value = 0.01 * (0.00617544789795f * powf(hue, 2) - 1.54124326627f * hue + 142.0f);
        return Terrain_HSVtoRGB(hue, min(0.3f, saturation), min(0.8f, value + 0.075f * max(0, logf(light / (1.0f - light)))));
    }
}

SDL_Color Terrain_MiniMapColor(float* map, int mapSize, int x, int y, float i)
{
    struct gradient grad = Perlin_GetGradient(map, mapSize, x, y);
    float light = 5.0f * grad.gradY;
    if (Terrain_IsBorder(map, mapSize, mapSize, x, y, 0.5f, 1)) {
        return Terrain_HSVtoRGB(196, 0.25, 0.85);
    } else if (i < 0.5) {
        return Terrain_HSVtoRGB(201, 0.05, 1);
    } else
        // Land
        if (i < 9 / 15.0) {
        return Terrain_HSVtoRGB(126 - light, 0.15 - light, 0.8 + light);
    } else if (i < 10 / 15.0) {
        return Terrain_HSVtoRGB(104 - light, 0.125 - light, 0.825 + light);
    } else if (i < 11 / 15.0) {
        return Terrain_HSVtoRGB(82 - light, 0.1 - light, 0.85 + light);
    } else if (i < 12 / 15.0) {
        return Terrain_HSVtoRGB(60 - light, 0.075 - light, 0.875 + light);
    } else {
        return Terrain_HSVtoRGB(38 - light, 0.05 - light, 0.9 + light);
    }
}

/*
	Sets the terrain offset and zoom depending on the game state's input */
void Terrain_Update(struct terrain* terrain)
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
void Terrain_Render(struct terrain* terrain)
{
    SDL_Rect rect = { 0, 0, 0, 0 };
    Terrain_Translate(&rect, 0, 0, 0, 0);
    rect.w = rect.h = (int)(terrain_zoom * terrain->size);
    SDL_RenderCopy(g->rend, terrain->texture, NULL, &rect);

    SDL_SetRenderDrawColor(g->rend, 0, 0, 0, min(max(50 * terrain_zoom, 12), 50));
    SDL_FPoint gridLineStart = { 32, 32 };
    SDL_Rect gridLineRect = { 0, 0, 0, 0 };
    Terrain_Translate(&gridLineRect, gridLineStart.x, gridLineStart.y, 64, 64);
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

void Terrain_FindPorts(struct terrain* terrain)
{
    for (int x = 32; x < terrain->size; x += 64) {
        for (int y = 32; y < terrain->size; y += 64) {
            Vector point = { x, y };
            if (Terrain_GetHeight(terrain, x, y) <= 0.5f && (Terrain_GetHeight(terrain, x - 64, y) > 0.5f || Terrain_GetHeight(terrain, x, y - 64) > 0.5f || Terrain_GetHeight(terrain, x + 64, y) > 0.5f || Terrain_GetHeight(terrain, x, y + 64) > 0.5f)) {
                Arraylist_Add(&terrain->ports, &point);
            }
        }
    }
}

static bool checkIsEmpty(struct terrain* terrain, int x, int y)
{
    return Terrain_GetHeight(terrain, x, y) > 0.5f && !terrain->continents[x + y * terrain->size];
}

static void addAdjacent(struct terrain* terrain, int x, int y, Arraylist** queue)
{
    Vector newSquare = { x, y };
    if (checkIsEmpty(terrain, x, y)) {
        terrain->continents[x + y * terrain->size] = terrain->numContinents;
        Arraylist_Add(queue, &newSquare);
    }
}

static void continentFloodFill(struct terrain* terrain, Vector square)
{
    Arraylist* queue = Arraylist_Create(10, sizeof(Vector));
    Arraylist_Add(&queue, &square);
    terrain->continents[(int)square.x + (int)square.y * terrain->size] = terrain->numContinents;

    while (queue->size > 0) {
        square = *(Vector*)Arraylist_Get(queue, 0);
        Arraylist_Remove(queue, 0);
        addAdjacent(terrain, square.x + 1, square.y, &queue);
        addAdjacent(terrain, square.x - 1, square.y, &queue);
        addAdjacent(terrain, square.x, square.y + 1, &queue);
        addAdjacent(terrain, square.x, square.y - 1, &queue);
    }
    Arraylist_Destroy(queue);
}

void Terrain_FindContinents(struct terrain* terrain)
{
    for (int x = 0; x < terrain->size; x++) {
        for (int y = 0; y < terrain->size; y++) {
            if (checkIsEmpty(terrain, x, y)) {
                terrain->numContinents++;
                continentFloodFill(terrain, (Vector) { x, y });
            }
        }
    }
}

// Perform dijkstra's algorithm for the two capital points, with sea/land transitions being incredibly expensive
// Mark out the continents that were traversed over, those are the prime continents that units should build ports in between
void Terrain_FindCapitalPath(struct terrain* terrain, Vector from, Vector to)
{
}

/*
	Returns the height at a given point (x, y). If point is outside bounds, 
	returns -1 */
float Terrain_GetHeight(struct terrain* terrain, int x, int y)
{
    if (x < 0 || y < 0 || x >= terrain->size || y >= terrain->size) {
        return -1.0f;
    }
    return terrain->map[y * terrain->size + x];
}

/*
	Sets the offset */
void Terrain_SetOffset(struct vector vector)
{
    offset.x = -vector.x;
    offset.y = -vector.y;
    oldOffset.x = offset.x;
    oldOffset.y = offset.y;
}

float Terrain_GetZoom()
{
    return terrain_zoom;
}

/*
* Returns the ore content of the tile at the point (x / 64, y / 64)
* 
* @param x	X coordinate on the map
* @param y	Y coordinate on the map
*/
float Terrain_GetOre(struct terrain* terrain, int x, int y)
{
    x /= 64;
    y /= 64;
    return terrain->ore[x + y * (terrain->tileSize)];
}

float Terrain_GetHeightForBuilding(struct terrain* terrain, int x, int y)
{
    return Terrain_GetHeight(terrain, 64 * (int)(x - 32) / 64 + 32, 64 * (int)(y - 32) / 64 + 32);
}

EntityID Terrain_GetBuildingAt(struct terrain* terrain, int x, int y)
{
    if (x < 0 || y < 0 || x >= terrain->size || y >= terrain->size) {
        return INVALID_ENTITY_INDEX;
    }
    x /= 64;
    y /= 64;
    return terrain->buildings[x + y * (terrain->tileSize)];
}

void Terrain_SetBuildingAt(struct terrain* terrain, EntityID id, int x, int y)
{
    x -= 32;
    y -= 32;
    x /= 64;
    y /= 64;
    terrain->buildings[x + y * (terrain->tileSize)] = id;
}

EntityID Terrain_GetWallAt(struct terrain* terrain, int x, int y)
{
    x /= 32;
    y /= 32;
    return terrain->walls[x + y * (terrain->tileSize)];
}

void Terrain_SetWallAt(struct terrain* terrain, EntityID id, int x, int y)
{
    x /= 32;
    y /= 32;
    terrain->walls[x + y * (terrain->tileSize)] = id;
}

int Terrain_ClosestBuildingDist(struct terrain* terrain, int x1, int y1)
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

int Terrain_ClosestMaskDist(struct scene* scene, ComponentKey key, struct terrain* terrain, int x1, int y1)
{
    x1 -= 32;
    y1 -= 32;
    x1 /= 64;
    y1 /= 64;
    int retval = terrain->size * 2;
    for (int x = 0; x < terrain->tileSize; x++) {
        for (int y = 0; y < terrain->tileSize; y++) {
            EntityID buildingID = terrain->buildings[x + y * terrain->tileSize];
            if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponents(scene, buildingID, key)) { // EntityHasComponents giving errors about entity version
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
EntityID Terrain_AdjacentMask(struct scene* scene, ComponentKey key, struct terrain* terrain, int x, int y)
{
    x /= 64;
    y /= 64;
    if (x > 0) {
        EntityID buildingID = terrain->buildings[(x - 1) + y * terrain->tileSize];
        if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponents(scene, buildingID, key)) {
            return buildingID;
        }
    }
    if (y > 0) {
        EntityID buildingID = terrain->buildings[x + (y - 1) * terrain->tileSize];
        if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponents(scene, buildingID, key)) {
            return buildingID;
        }
    }
    if (x < terrain->tileSize - 1) {
        EntityID buildingID = terrain->buildings[(x + 1) + y * terrain->tileSize];
        if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponents(scene, buildingID, key)) {
            return buildingID;
        }
    }
    if (y < terrain->tileSize - 1) {
        EntityID buildingID = terrain->buildings[x + (y + 1) * terrain->tileSize];
        if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponents(scene, buildingID, key)) {
            return buildingID;
        }
    }
    return INVALID_ENTITY_INDEX;
}

/*
	Takes in a pointer to an SDL_Rect struct, the position and size of a sprite. 
	Translates and scales the rect based on the offset and zoom 
	
	Converts map coords to screen coords*/
void Terrain_Translate(SDL_Rect* newPos, float x, float y, float width, float height)
{
    newPos->x = (int)((x + offset.x - width / 2.0f) * terrain_zoom + g->width / 2.0f);
    newPos->y = (int)((y + offset.y - height / 2.0f) * terrain_zoom + g->height / 2.0f);
    newPos->w = (int)(width * terrain_zoom);
    newPos->h = (int)(height * terrain_zoom);
}
struct vector Terrain_MousePos()
{
    return mousePos;
}

// Converts screen coords to map coords
SDL_FPoint Terrain_InverseTranslate(int x, int y)
{
    SDL_FPoint newPos;
    newPos.x = (x - g->width / 2) / terrain_zoom - offset.x;
    newPos.y = (y - g->height / 2) / terrain_zoom - offset.y;
    return newPos;
}

/*
This checks to see if a given point is a border between two values
*/
bool Terrain_IsBorder(float* terrain, int width, int height, int x, int y, float z, int i)
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
SDL_Color Terrain_HSVtoRGB(float hue, float saturation, float value)
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
bool Terrain_LineOfSight(struct terrain* terrain, Vector from, Vector to, float z)
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
        float height = Terrain_GetHeight(terrain, (int)check.x, (int)check.y);
        if (height >= z + 0.5f || height <= z) {
            return false;
        }
    }
    return true;
}

Vector Terrain_LineOfSightPoint(struct terrain* terrain, Vector from, Vector to, float z)
{
    if (to.x < 0 || to.x >= terrain->size || to.y < 0 || to.y >= terrain->size) {
        return from;
    }
    if (z > 0.5) {
        return to;
    }
    Vector increment = Vector_Scalar(Vector_Normalize(Vector_Sub(to, from)), 0.5);
    Vector check = { from.x, from.y };
    float distance = Vector_Dist(from, to);
    for (double i = 0; i < distance; i += 0.5) {
        check.x += increment.x;
        check.y += increment.y;
        float height = Terrain_GetHeight(terrain, (int)check.x, (int)check.y);
        if (height >= z + 0.5f || height <= z) {
            return check;
        }
    }
    return check;
}

bool Terrain_IsSolidSquare(struct terrain* terrain, Vector point)
{
    // Must be land
    if (!Terrain_LineOfSight(terrain, point, Vector_Add(point, (Vector) { 95, 0 }), 0.5))
        return false;
    if (!Terrain_LineOfSight(terrain, point, Vector_Add(point, (Vector) { -96, 0 }), 0.5))
        return false;
    if (!Terrain_LineOfSight(terrain, point, Vector_Add(point, (Vector) { 0, 95 }), 0.5))
        return false;
    if (!Terrain_LineOfSight(terrain, point, Vector_Add(point, (Vector) { 0, -96 }), 0.5))
        return false;
    return true;
}

/*
	Takes in a seed starting location. Searches the entire map for the location 
	that is closest to the starting point, and is above sea level */
struct vector Terrain_FindBestLocation(struct terrain* terrain, struct vector start)
{
    // Search for closest unvisited tile
    struct vector closestTile = { 0, 0 };
    float tempDist = FLT_MAX;
    for (int y = 0; y < terrain->size; y++) {
        for (int x = 0; x < terrain->size; x++) {
            struct vector point = { x * 64.0f + 32.0f, y * 64.0f + 32.0f };
            // Must be land
            if (!Terrain_LineOfSight(terrain, point, Vector_Add(point, (Vector) { 96, 0 }), 0.5))
                continue;
            if (!Terrain_LineOfSight(terrain, point, Vector_Add(point, (Vector) { -96, 0 }), 0.5))
                continue;
            if (!Terrain_LineOfSight(terrain, point, Vector_Add(point, (Vector) { 0, 96 }), 0.5))
                continue;
            if (!Terrain_LineOfSight(terrain, point, Vector_Add(point, (Vector) { 0, -96 }), 0.5))
                continue;
            float score = Vector_Dist(start, point);

            // Must have direct line of sight to tile center
            if (score < tempDist) {
                tempDist = score;
                closestTile = point;
            }
        }
    }

    return closestTile;
}