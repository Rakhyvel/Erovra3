/*
game.c
*/
#pragma once
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

struct game* game_init() {
	g = (struct game*)malloc(sizeof(struct game*));
	if (!g) {
		exit(1);
	}

	// Init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Error: Initializing SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Create Window
	g->window = SDL_CreateWindow("Erovra 3.0.2", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, 925, 510,
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	if (g->window == NULL) {
		printf("Error: Creating SDL window: %s\n", SDL_GetError());
		exit(1);
	}

	// Create Renderer
	g->rend = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED);
	if (g->rend == NULL) {
		printf("Error: Creating SDL renderer: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetRenderDrawBlendMode(g->rend, SDL_BLENDMODE_BLEND);

	g->running = 1;
	g->up = g->down = g->left = g->right = g->ctrl = g->mouseLeftDown = g->mouseRightDown = g->mouseLeftUp = g->mouseRightUp = g->mouseDrag = g->mouseDragged = 0;
}

void game_beginDraw() {
	SDL_SetRenderDrawColor(g->rend, 49, 46, 43, 255);
	SDL_RenderClear(g->rend);
}

void game_endDraw() {
	SDL_RenderPresent(g->rend);
}

void game_pollInput() {
	// PRE INPUT
	g->mouseLeftUp = false;
	g->mouseRightUp = false;

	// DIRECT INPUT
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			g->running = 0;
			break;
		case SDL_WINDOWEVENT:
			SDL_GetWindowSize(g->window, &g->width, &g->height);
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.scancode) {
			case SDL_SCANCODE_W:
				g->up = 1;
				break;
			case SDL_SCANCODE_S:
				g->down = 1;
				break;
			case SDL_SCANCODE_A:
				g->left = 1;
				break;
			case SDL_SCANCODE_D:
				g->right = 1;
				break;
			case SDL_SCANCODE_LCTRL:
				g->ctrl = 1;
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.scancode) {
			case SDL_SCANCODE_W:
				g->up = 0;
				break;
			case SDL_SCANCODE_S:
				g->down = 0;
				break;
			case SDL_SCANCODE_A:
				g->left = 0;
				break;
			case SDL_SCANCODE_D:
				g->right = 0;
				break;
			case SDL_SCANCODE_LCTRL:
				g->ctrl = 0;
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&(g->mouseX), &(g->mouseY));
			if (event.button.button == SDL_BUTTON_LEFT) {
				g->mouseDrag = true;
			}
			break;
		case SDL_MOUSEWHEEL:
			g->mouseWheelX += event.wheel.x;
			g->mouseWheelY += event.wheel.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button) {
			case SDL_BUTTON_LEFT:
				g->mouseLeftDown = true;
				SDL_GetMouseState(&(g->mouseInitX), &(g->mouseInitY));
				break;
			case SDL_BUTTON_RIGHT:
				g->mouseRightDown = true;
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button) {
			case SDL_BUTTON_LEFT:
				if (g->mouseLeftDown) {
					g->mouseLeftUp = true;
				}
				g->mouseLeftDown = false;
				g->mouseDrag = false;
				break;
			case SDL_BUTTON_RIGHT:
				if (g->mouseRightDown) {
					g->mouseRightUp = true;
				}
				g->mouseRightDown = false;
				break;
			}
			break;
		}
	}

	// POST INPUT
	g->mouseDragged = abs(g->mouseInitX - g->mouseX) > 16 || abs(g->mouseInitY - g->mouseY) > 16;
}