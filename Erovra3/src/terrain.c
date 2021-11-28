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
#include <time.h>

#include "./engine/apricot.h"
#include "./util/debug.h"
#include "./util/heap.h"
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
        retval->ore[i] *= 0.5f * (retval->map[x + y * mapSize] + 0.75f) + 0.25f;
    }
    retval->continents = calloc(retval->size * retval->size, sizeof(int));
    if (!retval->continents) {
        PANIC("Memory error");
    }
    retval->continentPoints = Arraylist_Create(5, sizeof(Vector));
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
    retval->keyContinents = Arraylist_Create(5, sizeof(int));
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
    Arraylist_Destroy(terrain->keyContinents);
    SDL_DestroyTexture(terrain->texture);
    free(terrain);
}

SDL_Color Terrain_RealisticColor(float* map, int mapSize, int x, int y, float i)
{
    i = i * 2 - 0.5f;

    if (mapSize >= 512.0f && Terrain_IsBorder(map, mapSize, mapSize, x, y, 0.5f, 1)) {
        return (SDL_Color) { 255, 255, 255 };
    } else if (i < 0.5) {
        // water
        i *= 2;
        i = powf(i, 51);
        Gradient g = Perlin_GetGradient(map, mapSize, x, y);
        return Terrain_HSVtoRGB(214.0f - 25.0f * i, 1.0f - 0.45f * i, 0.21f + 0.6f * i + 0.1f * powf(i, 91) + g.gradY * 7);
    } else {
        // ground
        i = (i - 0.5f) * 2;
        i = powf(i, 0.25f);
        // ranges from 0 to 1, 0 being completely dark, 1 being completely illuminated
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
            // vector q = a-b
            float q_x = a_x - b_x;
            float q_y = a_y - b_y;
            float q_z = a_z - b_z;
            // vector r = a-c
            float r_x = a_x - c_x;
            float r_y = a_y - c_y;
            float r_z = a_z - c_z; // +((float)rand()) / ((float)RAND_MAX) * q_z * 2 - q_z * 2 * 0.5f;
            // normal = qxr
            float n_x = (q_y * r_z) - (q_z * r_y);
            float n_y = (q_z * r_x) - (q_x * r_z);
            float n_z = (q_x * r_y) - (q_y * r_x);
            // normalize
            float mag = n_x * n_x + n_y * n_y + n_z * n_z;
            if (mag != 0) {
                n_y /= mag;
                n_z /= mag;
            }
            // light = to_sun . normal
            light = 0.0f * n_x - 0.7071f * n_y + 0.7071f * n_z;
        }
        i *= 100;
        float hue = max(30, min(120, 0.0000000369 * powf(i, 5) - 0.00000714407361075 * powf(i, 4) + 0.000381833965482 * powf(i, 3) - 0.00120026434352 * powf(i, 2) + 0.207995982732 * i + 35.3635585447));
        float saturation = 0.01 * (0.00000104466482419f * powf(hue, 5) - 0.000370237098314f * powf(hue, 4) + 0.0514055142232 * powf(hue, 3) - 3.4855548673f * powf(hue, 2) + 115.271785291 * hue - 1464.19348868);
        float value = 0.01 * (0.00617544789795f * powf(hue, 2) - 1.54124326627f * hue + 160.0f);
        return Terrain_HSVtoRGB(hue + 120 * (1 - light) - 20, min(0.3f, saturation), max(0.2f, min(0.8f, value * light)));
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
    terrain_zoom *= (float)pow(1.1, Apricot_MouseWheel.y - oldWheel);
    oldWheel = Apricot_MouseWheel.y;
    if (terrain_zoom < 0.75 * Apricot_Height / terrain->size) {
        terrain_zoom = 0.75f * Apricot_Height / terrain->size;
    }
    if (terrain_zoom > 8.0f * Apricot_Height / terrain->size) {
        terrain_zoom = 8.0f * Apricot_Height / terrain->size;
    }

    if (Apricot_MouseLeftDown && !mouseDown) {
        mouseDown = true;
    }
    if (!Apricot_MouseLeftDown && mouseDown) {
        mouseDown = false;
        oldOffset = offset;
    }
    if (Apricot_MouseMoved) {
        mousePos.x = (float)(((float)Apricot_MousePos.x - (float)Apricot_Width / 2.0f) / terrain_zoom) - offset.x;
        mousePos.y = (float)(((float)Apricot_MousePos.y - (float)Apricot_Height / 2.0f) / terrain_zoom) - offset.y;
    }
    if (Apricot_MouseDrag && !Apricot_Keys[SDL_SCANCODE_LSHIFT]) {
        offset = Vector_Add(Vector_Scalar(Vector_Sub(Apricot_MousePos, Apricot_MouseInit), 1 / terrain_zoom), oldOffset);
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
                    isAdjacent |= !Terrain_LineOfSight(terrain, (Vector) { x, y }, (Vector) { x + 32 * x0, y + 32 * y0 }, 0.0f);
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
                Vector point = (Vector) { x, y };
                terrain->numContinents++;
                continentFloodFill(terrain, point);
                Arraylist_Add(&terrain->continentPoints, &point);
            }
        }
    }
}

void proccessChildren(struct terrain* terrain, float cost, int* crossings, int u, int v, Heap* pq, float* dist, Sint32* parent, bool* processed)
{
    // Update dist[v] only if is not processed, there is an
    // edge from u to v, and total weight of path from src to
    // v through u is smaller than current value of dist[v]
    if ((terrain->map[u] <= 0.5f) != (terrain->map[v] <= 0.5f)) {
        cost += 4 * terrain->size;
        crossings++;
    }
    if (!processed[v] && dist[u] + cost < dist[v]) {
        Heap_Insert(pq, dist[u] + cost, v);
        dist[v] = dist[u] + cost;
        parent[v] = u;
    }
}

// Perform dijkstra's algorithm for the two capital points, with sea/land transitions being incredibly expensive
// Mark out the continents that were traversed over, those are the prime continents that units should build ports in between
struct dijkstrasResult Terrain_Dijkstra(struct terrain* terrain, Vector from, Vector to)
{
    Heap* pq = Heap_Create(terrain->size * terrain->size);
    float* dist = (float*)malloc(terrain->size * terrain->size * sizeof(float));
    if (!dist) {
        PANIC("Mem error");
    }
    Sint32* parent = (Sint32*)malloc(terrain->size * terrain->size * sizeof(Sint32));
    if (!parent) {
        PANIC("Mem error");
    }
    bool* processed = (bool*)malloc(terrain->size * terrain->size * sizeof(bool));
    if (!processed) {
        PANIC("Mem error");
    }
    int crossings = 0;

    for (int i = 0; i < terrain->size * terrain->size; i++) {
        dist[i] = terrain->size * terrain->size;
        processed[i] = false;
        parent[i] = -1;
    }
    parent[(int)from.x + (int)from.y * terrain->size] = -1;
    dist[(int)from.x + (int)from.y * terrain->size] = 0;
    Heap_Insert(pq, 0, (Uint32)from.x + (Uint32)from.y * (Uint32)terrain->size);

    while (pq->size >= 0) {
        int u = Heap_GetMin(pq);
        Heap_Remove(pq, 0);
        int x = (int)(u % terrain->size);
        int y = (int)(u / terrain->size);

        if (x == (int)to.x && y == (int)to.y) {
            break;
        }

        processed[u] = true;
        for (int x0 = -1; x0 <= 1; x0++) {
            for (int y0 = -1; y0 <= 1; y0++) {
                if (x0 != y0 && x + x0 >= 0 && x + x0 < terrain->size && y + y0 >= 0 && y + y0 < terrain->size) {
                    Vector point = { x + x0, y + y0 };
                    proccessChildren(terrain, x0 * x0 + y0 * y0 + Vector_Dist(to, point), &crossings, u, u + x0 + y0 * terrain->size, pq, dist, parent, processed);
                }
            }
        }
    }

    Heap_Destroy(pq);
    free(processed);
    return (struct dijkstrasResult) { dist, parent };
}

/*
	Takes two positions, checks to see if there is dry land between them */
int Terrain_LineOfSightSeaToLand(struct terrain* terrain, Vector from, Vector increment)
{
    Vector check = { from.x, from.y };
    while (Terrain_GetHeight(terrain, (int)check.x, (int)check.y) != -1) {
        check.x += increment.x;
        check.y += increment.y;
        float height = Terrain_GetHeight(terrain, (int)check.x, (int)check.y);
        if (height > 0.5f) {
            return terrain->continents[(int)check.x + (int)check.y * terrain->size];
        }
    }
    return -1;
}

void Terrain_FindCapitalPath(struct terrain* terrain, Vector from, Vector to)
{
    clock_t time = clock();
    struct dijkstrasResult path = Terrain_Dijkstra(terrain, from, to);
    int p = path.parent[(int)to.x + (int)to.y * terrain->size];
    while (p != -1) {
        int continent = terrain->continents[p];
        if (continent && !Arraylist_Contains(terrain->keyContinents, &continent)) {
            Arraylist_Add(&terrain->keyContinents, &continent);
        }
        p = path.parent[p];
    }
    free(path.parent);
    free(path.dist);
    time = clock() - time;
    printf("%f\n", ((double)time) / CLOCKS_PER_SEC);

    if (terrain->keyContinents->size < 2) {
        Arraylist_Clear(terrain->ports);
        printf("No key continents\n");
        return;
    }
    // Go through each port point, check to see if they are valid (can see (dist is less than 8 times the size) more than one key continent)
    for (int i = 0; i < terrain->ports->size; i++) {
        printf("%d: ", i);
        Vector portPoint = *(Vector*)Arraylist_Get(terrain->ports, i);
        Arraylist* keyContinentsSeen = Arraylist_Create(10, sizeof(int));
        for (int x = -10; x <= 10; x++) {
            for (int y = -10; y <= 10; y++) {
                if (x == 0 && y == 0)
                    continue;

                int continentSeen = Terrain_LineOfSightSeaToLand(terrain, portPoint, Vector_Normalize((Vector) { x, y }));
                if (!Arraylist_Contains(keyContinentsSeen, &continentSeen) && Arraylist_Contains(terrain->keyContinents, &continentSeen)) {
                    Arraylist_Add(&keyContinentsSeen, &continentSeen);
                }
                if (keyContinentsSeen->size >= 2) {
                    break;
                }
            }
        }
        if (keyContinentsSeen->size < 2) {
            Arraylist_Remove(terrain->ports, i);
            i--;
        }
        printf("\n");
    }
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
SDL_FPoint Terrain_InverseTranslate(int x, int y)
{
    SDL_FPoint newPos;
    newPos.x = (x - Apricot_Width / 2) / terrain_zoom - offset.x;
    newPos.y = (y - Apricot_Height / 2) / terrain_zoom - offset.y;
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