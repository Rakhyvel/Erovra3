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
#include <time.h>

#include "./engine/apricot.h"
#include "./util/debug.h"
#include "./util/heap.h"
#include "./util/noise.h"
#include "./util/vector.h"
#include "terrain.h"

static bool mouseDown = false;
static struct vector offset = { 0, 0 };
static struct vector oldOffset = { 0, 0 };
static struct vector prevOffset = { 0, 0 };
static struct vector mousePos = { 0, 0 };
static Vector mouseVel = { 0, 0 };
static float terrain_zoom_target = 1;
static float terrain_zoom = 0.8f;
static int oldWheel = 0;

/*
	Creates the terrain struct, with the height map, ore map, and terrain 
	texture. */
struct terrain* Terrain_Create(int mapSize, float* map, float* trees, float* ore, SDL_Texture* texture, int* status)
{
    struct terrain* retval = calloc(1, sizeof(struct terrain));
    if (!retval) {
        PANIC("Memory error Terrain_Create() creating the terrain\n");
    }
    retval->size = mapSize;
    retval->tileSize = mapSize / 64;
    retval->map = map;
    retval->texture = texture;

    retval->ore = ore;
    retval->timber = trees;

    retval->continents = calloc(retval->size * retval->size, sizeof(int));
    retval->oceans = calloc(retval->size * retval->size, sizeof(int));
    retval->buildings = (EntityID*)malloc(retval->tileSize * retval->tileSize * sizeof(EntityID));
    for (int i = 0; i < (retval->tileSize * retval->tileSize); i++) {
        retval->buildings[i] = INVALID_ENTITY_INDEX;
    }
    retval->walls = (EntityID*)malloc(((mapSize + 1) * (mapSize + 1)) / (32 * 32) * sizeof(EntityID));
    for (int i = 0; i < ((mapSize + 1) * (mapSize + 1)) / (32 * 32); i++) {
        retval->walls[i] = INVALID_ENTITY_INDEX;
    }
    retval->ports = Arraylist_Create(10, sizeof(Vector));
    Terrain_FindPorts(retval);
    Terrain_FindContinentsAndOceans(retval, status);
    return retval;
}

void Terrain_Destroy(struct terrain* terrain)
{
    // DO NOT free map, timber, ore. Are freed by menu scene before each generation
    free(terrain->continents);
    free(terrain->oceans);
    free(terrain->buildings);
    free(terrain->walls);
    Arraylist_Destroy(terrain->ports);
    SDL_DestroyTexture(terrain->texture);
    free(terrain);
}

SDL_Color Terrain_RealisticColor(float* map, int mapSize, int x, int y, float i)
{
    i = i * 2 - 0.5f;

    struct gradient grad = Noise_GetGradient(map, mapSize, x, y);
    float light = 7.0f * grad.gradY;
    light = min(0.05, light) * mapSize * 0.001;

    if (mapSize >= 512.0f && Terrain_IsBorder(map, mapSize, mapSize, x, y, 0.5f, 1)) {
        return (SDL_Color) { 255, 255, 255 };
    } else if (i <= 0.5) {
        // water
        i *= 2;
        i = powf(i, 91);
        return Terrain_HSVtoRGB(206 - 20 * i - light, 0.75 - 0.4 * i - light, 0.3 + 0.3 * i + light);
    } else {
        // ground
        i = (i - 0.5f) * 2;
        i = powf(i, 0.5);
        return Terrain_HSVtoRGB(50 + 60 * i - light, 0.2 + i * 0.25 - light, .7 - i * 0.4 + light);
    }
}

SDL_Color Terrain_MiniMapColor(float* map, int mapSize, int x, int y, float i)
{
    struct gradient grad = Noise_GetGradient(map, mapSize, x, y);
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
    terrain_zoom *= (float)pow(1.1, Apricot_MouseWheel.y - oldWheel);
    oldWheel = Apricot_MouseWheel.y;
#define MAX_ZOOM 0.75
    if (terrain_zoom < MAX_ZOOM * Apricot_Height / 1024.0f) {
        terrain_zoom = MAX_ZOOM * Apricot_Height / 1024.0f;
    }
    if (terrain_zoom > 4.0f * Apricot_Height / 1024.0f) {
        terrain_zoom = 4.0f * Apricot_Height / 1024.0f;
    }

    if (Apricot_MouseMoved) {
        mousePos.x = (float)(((float)Apricot_MousePos.x - (float)Apricot_Width / 2.0f) / terrain_zoom) - offset.x;
        mousePos.y = (float)(((float)Apricot_MousePos.y - (float)Apricot_Height / 2.0f) / terrain_zoom) - offset.y;
    }
    if (Apricot_MouseDrag && !Apricot_Keys[SDL_SCANCODE_LSHIFT]) {
        prevOffset = offset;
        offset = Vector_Add(Vector_Scalar(Vector_Sub(Apricot_MousePos, Apricot_MouseInit), 1 / terrain_zoom), oldOffset);
        mouseVel = Vector_Sub(offset, prevOffset);
    } else if (!Apricot_MouseLeftDown) {
        offset = Vector_Add(mouseVel, offset);
        oldOffset = offset;
        mouseVel = Vector_Scalar(mouseVel, 0.9);
    } else {
        mouseVel = (Vector) { 0, 0 };
    }
}

/*
	Renders the terrain's texture to the screen, and some grid lines */
void Terrain_Render(struct terrain* terrain)
{
    if (terrain->texture) {
        SDL_Rect rect = { 0, 0, 0, 0 };
        Terrain_Translate(&rect, 0, 0, 0, 0);
        rect.w = rect.h = (int)(terrain_zoom * terrain->size);
        SDL_RenderCopy(Apricot_Renderer, terrain->texture, NULL, &rect);

        SDL_SetRenderDrawColor(Apricot_Renderer, 0, 0, 0, min(max(50 * terrain_zoom, 12), 50));
        SDL_FPoint gridLineStart = { 32, 32 };
        SDL_Rect gridLineRect = { 0, 0, 0, 0 };
        Terrain_Translate(&gridLineRect, gridLineStart.x, gridLineStart.y, 64, 64);
        for (int x = 0; x <= terrain->tileSize; x++) {
            SDL_RenderDrawLine(Apricot_Renderer,
                max((int)(gridLineRect.x + x * 64.0 * terrain_zoom), 0),
                gridLineRect.y,
                (int)(gridLineRect.x + x * 64.0 * terrain_zoom),
                (int)(gridLineRect.y + (terrain->size * terrain_zoom)));
        }
        for (int y = 0; y <= terrain->tileSize; y++) {
            SDL_RenderDrawLine(Apricot_Renderer,
                max(gridLineRect.x, 0),
                (int)(gridLineRect.y + y * 64.0 * terrain_zoom),
                (int)(gridLineRect.x + (terrain->size * terrain_zoom)),
                (int)(gridLineRect.y + y * 64.0 * terrain_zoom));
        }

        SDL_RenderDrawRect(Apricot_Renderer, &rect);
        rect.x += 1;
        rect.y += 1;
        rect.w -= 2;
        rect.h -= 2;
        SDL_RenderDrawRect(Apricot_Renderer, &rect);
    }
}

void Terrain_FindPorts(struct terrain* terrain)
{
    for (int x = 32; x < terrain->size; x += 64) {
        for (int y = 32; y < terrain->size; y += 64) {
            Vector point = { x, y };
            bool isAdjacent = false;
            for (int x0 = -1; x0 <= 1; x0++) {
                for (int y0 = -1; y0 <= 1; y0++) {
                    isAdjacent |= !Terrain_LineOfSight(terrain, (Vector) { x, y }, (Vector) { x + 32 * x0, y + 32 * y0 }, 0.0f)
                        && Terrain_LineOfSight(terrain, (Vector) { x + 32 * x0, y + 32 * y0 }, (Vector) { x + 64 * x0, y + 64 * y0 }, 0.5f);
                }
            }
            if (isAdjacent) {
                Arraylist_Add(&terrain->ports, &point);
            }
        }
    }
}

static bool checkIsEmpty(struct terrain* terrain, int x, int y)
{
    return Terrain_GetHeight(terrain, x, y) >= 0.5f && !terrain->continents[x + y * terrain->size];
}

static void addAdjacent(struct terrain* terrain, int x, int y, Arraylist** queue, int* status)
{
    Vector newSquare = { x, y };
    if (checkIsEmpty(terrain, x, y)) {
        terrain->continents[x + y * terrain->size] = terrain->numContinents;
        Arraylist_Add(queue, &newSquare);
        (*status)++;
    }
}

static void addAdjacentOcean(struct terrain* terrain, int x, int y, Arraylist** queue, int* status)
{
    Vector newSquare = { x, y };
    if (Terrain_GetHeight(terrain, x, y) > 0) {
        if (Terrain_GetHeight(terrain, x, y) < 0.5f) {
            if (!terrain->oceans[x + y * terrain->size]) {
                terrain->oceans[x + y * terrain->size] = terrain->numOceans;
                Arraylist_Add(queue, &newSquare);
                (*status)++;
            }
        } else {
            // Set (terrain->continent[x, y], numOceans) in adjacency matrix to true
            bool* row = Arraylist_Get(terrain->contOceanAdjacency, terrain->numOceans);
            row[terrain->continents[x + y * terrain->size]] = true;
        }
    }
}

static void continentFloodFill(struct terrain* terrain, Vector square, int* status)
{
    Arraylist* queue = Arraylist_Create(10, sizeof(Vector));
    Arraylist_Add(&queue, &square);
    terrain->continents[(int)square.x + (int)square.y * terrain->size] = terrain->numContinents;

    while (queue->size > 0) {
        square = *(Vector*)Arraylist_Get(queue, 0);
        Arraylist_Remove(queue, 0);
        addAdjacent(terrain, square.x + 1, square.y, &queue, status);
        addAdjacent(terrain, square.x - 1, square.y, &queue, status);
        addAdjacent(terrain, square.x, square.y + 1, &queue, status);
        addAdjacent(terrain, square.x, square.y - 1, &queue, status);
    }
    Arraylist_Destroy(queue);
}

static void oceanFloodFill(struct terrain* terrain, Vector square, int* status)
{
    Arraylist* queue = Arraylist_Create(10, sizeof(Vector));
    Arraylist_Add(&queue, &square);
    terrain->oceans[(int)square.x + (int)square.y * terrain->size] = terrain->numOceans;

    while (queue->size > 0) {
        square = *(Vector*)Arraylist_Get(queue, 0);
        Arraylist_Remove(queue, 0);
        addAdjacentOcean(terrain, square.x + 1, square.y, &queue, status);
        addAdjacentOcean(terrain, square.x - 1, square.y, &queue, status);
        addAdjacentOcean(terrain, square.x, square.y + 1, &queue, status);
        addAdjacentOcean(terrain, square.x, square.y - 1, &queue, status);
    }
    Arraylist_Destroy(queue);
}

void Terrain_FindContinentsAndOceans(struct terrain* terrain, int* status)
{
    for (int x = 0; x < terrain->size; x++) {
        for (int y = 0; y < terrain->size; y++) {
            if (terrain->map[x + y * terrain->size] >= 0.5f && !terrain->continents[x + y * terrain->size]) {
                Vector point = (Vector) { x, y };
                terrain->numContinents++;
                continentFloodFill(terrain, point, status);
            }
        }
    }
    terrain->contOceanAdjacency = Arraylist_Create(10, sizeof(bool) * terrain->numContinents);
    // Add a dumby row
    bool* row = calloc(terrain->numContinents, sizeof(bool));
    Arraylist_Add(&terrain->contOceanAdjacency, row);
    free(row);
    for (int x = 0; x < terrain->size; x++) {
        for (int y = 0; y < terrain->size; y++) {
            if (terrain->map[x + y * terrain->size] < 0.5f && !terrain->oceans[x + y * terrain->size]) {
                Vector point = (Vector) { x, y };
                terrain->numOceans++;
                bool* row = calloc(terrain->numContinents, sizeof(bool));
                Arraylist_Add(&terrain->contOceanAdjacency, row);
                free(row);
                oceanFloodFill(terrain, point, status);
            }
        }
    }
}

void Terrain_EliminateUselessPortPoints(struct terrain* terrain, Arraylist* capitalPoints)
{
    bool* continentMap = calloc((terrain->numContinents + 1), sizeof(bool));
    for (int i = 0; i < capitalPoints->size; i++) {
        Vector capitalPoint = *(Vector*)Arraylist_Get(capitalPoints, i);
        continentMap[terrain->continents[(int)capitalPoint.x + (int)capitalPoint.y * terrain->size]] = true;
    }
    // Go through each port point, check to see if they are valid (can see (dist is less than 8 times the size) more than one key continent)
    for (int i = 0; i < terrain->ports->size; i++) {
        Vector portPoint = *(Vector*)Arraylist_Get(terrain->ports, i);
        int keyAdj = 0;
        bool* row = Arraylist_Get(terrain->contOceanAdjacency, terrain->oceans[(int)portPoint.x + (int)portPoint.y * terrain->size]);
        // Count how many key continents the port's ocean touches
        for (int j = 0; j < terrain->numContinents; j++) {
            if (row[j] && continentMap[j]) {
                keyAdj++;
            }
        }

        if (keyAdj < 2) {
            Arraylist_Remove(terrain->ports, i);
            i--;
        }
    }
    free(continentMap);
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
    prevOffset = offset;
}

float Terrain_GetZoom()
{
    return terrain_zoom;
}


float Terrain_GetTimber(struct terrain* terrain, int x, int y)
{
    x /= 64;
    y /= 64;
    if (x < 0 || y < 0 || x >= 2 * terrain->tileSize - 1 || y >= 2 * terrain->tileSize - 1) {
        return 0;
    }
    x /= 2;
    y /= 2;
    return (terrain->timber[x + y * (terrain->tileSize * 2)]
               + terrain->timber[x + 1 + y * (terrain->tileSize * 2)]
               + terrain->timber[x + (y + 1) * (terrain->tileSize * 2)]
               + terrain->timber[x + 1 + (y + 1) * (terrain->tileSize * 2)])
        / 4.0f;
}

float Terrain_GetCoal(struct terrain* terrain, int x, int y)
{
    x /= 64;
    y /= 64;
    if (x < 0 || y < 0 || x >= 2 * terrain->tileSize - 1 || y >= 2 * terrain->tileSize - 1) {
        return 0;
    }
    x /= 2;
    y /= 2;
    return -(max(0, -terrain->ore[x + y * (terrain->tileSize * 2)])
               + max(0, -terrain->ore[x + 1 + y * (terrain->tileSize * 2)])
               + max(0, -terrain->ore[x + (y + 1) * (terrain->tileSize * 2)])
               + max(0, -terrain->ore[x + 1 + (y + 1) * (terrain->tileSize * 2)]))
        / 4.0f;
}

/*
* Returns the ore content of the tile at the point (x / 32, y / 32)
* 
* @param x	X coordinate on the map
* @param y	Y coordinate on the map
*/
float Terrain_GetOre(struct terrain* terrain, int x, int y)
{
    x /= 64;
    y /= 64;
    if (x < 0 || y < 0 || x >= 2 * terrain->tileSize - 1 || y >= 2 * terrain->tileSize - 1) {
        return 0;
    }
    x /= 2;
    y /= 2;
    return (max(0, terrain->ore[x + y * (terrain->tileSize * 2)])
               + max(0, terrain->ore[x + 1 + y * (terrain->tileSize * 2)])
               + max(0, terrain->ore[x + (y + 1) * (terrain->tileSize * 2)])
               + max(0, terrain->ore[x + 1 + (y + 1) * (terrain->tileSize * 2)]))
        / 4.0f;
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
    x1 /= 64;
    y1 /= 64;
    int retval = terrain->size * 2;
    for (int x = 0; x < terrain->tileSize; x++) {
        for (int y = 0; y < terrain->tileSize; y++) {
            EntityID buildingID = terrain->buildings[x + y * terrain->tileSize];
            if (buildingID != INVALID_ENTITY_INDEX && Scene_EntityHasComponentMask(scene, Scene_CreateMask(scene, 1, key), buildingID)) { // EntityHasComponents giving errors about entity version
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
    newPos->x = (int)round((x + offset.x - width / 2.0f) * terrain_zoom + Apricot_Width / 2.0f);
    newPos->y = (int)round((y + offset.y - height / 2.0f) * terrain_zoom + Apricot_Height / 2.0f);
    newPos->w = (int)(width * terrain_zoom);
    newPos->h = (int)(height * terrain_zoom);
}
struct vector Terrain_MousePos()
{
    return mousePos;
}

// Converts screen coords to map coords
SDL_Point Terrain_InverseTranslate(int x, int y)
{
    SDL_Point newPos;
    newPos.x = (((float)x - (float)Apricot_Width / 2.0f) / terrain_zoom) - offset.x;
    newPos.y = (((float)y - (float)Apricot_Height / 2.0f) / terrain_zoom) - offset.y;
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
    SDL_Color color = { (Uint8)((r + m) * 255), (Uint8)((g + m) * 255), (Uint8)((b + m) * 255), 255 };
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
    Vector increment = Vector_Scalar(Vector_Normalize(Vector_Sub(to, from)), 0.2f);
    Vector check = { from.x, from.y };
    float distance = Vector_Dist(from, to);
    for (double i = 0; i < distance; i += 0.2f) {
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