#pragma once
#include "../engine/apricot.h"
#include "../engine/scene.h"
#include "../terrain.h"
#include "../util/debug.h"
#include "../util/perlin.h"
#include "./match.h"
#include <stdio.h>
#include <time.h>

Lexicon* lexicon;
float* map;

void Tournament_Update(Scene* scene)
{
}

Scene* Tournament_Init()
{
    Apricot_ASAP = true;
    int status;
    lexicon = Lexicon_Create("res/countryNames.txt", &status);
    Scene* tournament = Scene_Create(NULL, &Tournament_Update, NULL, NULL);
    Apricot_PushScene(tournament);

    map = Perlin_Generate(1024, 256, 24, &status);
    for (int x = 0; x < 1024; x++) {
        for (int y = 0; y < 1024; y++) {
            map[x + y * 1024] = 0.5f * map[x + y * 1024] + 0.5f;
        }
    }
    Terrain* terrain = Terrain_Create(1024, map, NULL);

    while (1) {
        clock_t time = clock();
        float continentSum = 0;
        struct dijkstrasResult path = Terrain_Dijkstra(terrain, (Vector) { 15 * 64 + 32, 15 * 64 + 32 }, (Vector) { 32, 32 });
        time = clock() - time;
        continentSum = path.dist[0];
        printf("Dijkstra's algorithm: %f %f\n", ((double)time) / CLOCKS_PER_SEC, continentSum);
        free(path.parent);
        free(path.dist);
    }

    return tournament;
}