#pragma once
/*
game.h
*/

#include <SDL.h>
#include <stdbool.h>
#include "scene.h"
#include "../util/arraylist.h"

struct game {
    SDL_Window* window;
    SDL_Renderer* rend;

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
		mouseLeftUp, 
		mouseRightUp, 
		mouseDrag, 
		mouseDragged;
    float dt;

	Arraylist* sceneStack;
};

struct game* g;

void Game_Init(char* windowName, int width, int height);
void Game_PushScene(Scene*);
void Game_PopScene(Scene* scene);
void Game_Run();
void Game_PollInput();