#pragma once
#include "../engine/gameState.h"
#include "../engine/scene.h"
#include "./match.h"
#include <stdio.h>

Lexicon* lexicon;
float* map;

void Tournament_Update()
{
    printf("Starting up new match...\n");
    Match_Init(map, "Tournament", lexicon, 16 * 64, true);
}

Scene* Tournament_Init()
{
    asap = true;
    int status;
    map = malloc(16 * 16 * 64 * 64 * sizeof(float));
    for (int i = 0; i < 16 * 16 * 64 * 64; i++) {
        map[i] = 0.75f;
    }
    lexicon = Lexicon_Create("res/countryNames.txt", &status);
    Scene* tournament = Scene_Create(NULL, &Tournament_Update, NULL, NULL);
    Game_PushScene(tournament);

    return tournament;
}