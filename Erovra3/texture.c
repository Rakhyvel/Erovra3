#pragma once
#include <SDL_image.h>
#include <stdio.h>

#include "gameState.h"
#include "texture.h"

void drawTexture(SDL_Texture* texture, int x, int y)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.h, &dest.w);
	SDL_RenderCopy(g->rend, texture, NULL, &dest);
}

SDL_Texture* loadTexture(char* filename)
{
	SDL_Surface* surface = IMG_Load(filename);
	if (!surface) {
		printf("Error: %s\n", IMG_GetError());
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(g->rend, surface);
	SDL_FreeSurface(surface);
	return texture;
}