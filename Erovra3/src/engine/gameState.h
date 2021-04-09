#pragma once
/*
game.h
*/

#include <SDL.h>
#include <stdbool.h>

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
		height;
    bool running, 
		up, 
		down, 
		left, 
		right, 
		ctrl, 
		shift, 
		keys[255],
		mouseMoved, 
		mouseLeftDown,
		mouseRightDown, 
		mouseLeftUp, 
		mouseRightUp, 
		mouseDrag, 
		mouseDragged;
};

struct game* g;

void Game_Init(char* windowName, int width, int height);
void Game_PollInput();
void Game_BeginDraw();
void Game_EndDraw();