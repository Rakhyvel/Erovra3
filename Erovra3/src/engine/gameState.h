#pragma once
/*
game.h
*/

#include "../util/arraylist.h"
#include "scene.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdbool.h>

#define GAME_POP_SCENE(i) \
    Game_PopScene(i);     \
    return;

extern bool asap;

struct game {
    const SDL_Window* window;
    const SDL_Renderer* rend;

    int mouseX,
        mouseY,
        mouseWheelX,
        mouseWheelY,
        mouseInitX,
        mouseInitY,
        width,
        height,
        ticks;

    bool running,
        up,
        down,
        left,
        right,
        ctrl,
        shift,
        lt,
        gt,
        keys[255],
        mouseMoved,
        mouseLeftDown,
        mouseRightDown,
        mouseLeftUp, // Left mouse button previously pressed, on this tick is released.
        mouseRightUp, // Right mouse button previously pressed, on this tick is released.
        mouseDrag,
        mouseDragged;
    char keyDown;

    double dt;

    Arraylist* sceneStack;

    Mix_Chunk* samples[2];
};

struct game* g;

void Game_Init(char* windowName, int width, int height);
void Game_Exit();
void Game_PushScene(Scene*);
/* Make sure to return immediately after calling this function! */
void Game_PopScene(int numScenes);
void Game_Run();
void Game_PollInput();