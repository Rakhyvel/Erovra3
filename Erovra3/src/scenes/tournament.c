#pragma once
#include "../engine/apricot.h"
#include "../engine/scene.h"
#include "../terrain.h"
#include "../util/debug.h"
#include "../util/noise.h"
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
    Scene* tournament = Scene_Create(NULL, &Tournament_Update, NULL, NULL);
    Apricot_PushScene(tournament);

    map = Noise_Generate(1024, 256, 24, &status);
    for (int x = 0; x < 1024; x++) {
        for (int y = 0; y < 1024; y++) {
            map[x + y * 1024] = 0.5f * map[x + y * 1024] + 0.5f;
        }
    }
    Terrain* terrain = Terrain_Create(1024, map, NULL, NULL, NULL, NULL);

    while (1) {
        clock_t time = clock();
        float continentSum = 0;
    }

    return tournament;
}