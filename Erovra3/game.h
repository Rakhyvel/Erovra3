#pragma once
/*
game.h
*/

#include <SDL.h>
#include <stdbool.h>

struct game {
	SDL_Window* window;
	SDL_Renderer* rend;

	int mouseX, mouseY, mouseWheelX, mouseWheelY, mouseInitX, mouseInitY, width, height;
	bool up, down, left, right, ctrl, mouseLeftDown, mouseRightDown, mouseLeftUp, mouseRightUp, mouseDrag, mouseDragged;
};

struct game* g;

struct game* game_init();
void game_pollInput();
void game_beginDraw();
void game_endDraw();