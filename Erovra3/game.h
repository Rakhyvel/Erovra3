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

struct game* game_init();
void game_pollInput(struct game*);
void game_beginDraw(struct game*);
void game_endDraw(struct game*);