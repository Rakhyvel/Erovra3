#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

#include "debug.h"
#include "gameState.h"
#include "textureManager.h"

SDL_Texture* loadTexture(char* filename);

static SDL_Texture* textures[MAX_TEXTURES];
static int numTextures = 0;

/*
	Changes the ColorMod of a texture given a texture id. */
void Texture_ColorMod(TextureID textureID, Uint8 r, Uint8 g, Uint8 b) 
{
	SDL_Texture* texture = textures[textureID];
	SDL_SetTextureColorMod(texture, r, g, b);
}

/*
	Draws a texture given the texture id at a position. 
	Image width and height will be originalSize * scalar. Scalars under 0 are ignored */
void Texture_Draw(TextureID textureID, int x, int y, float w, float h)
{
	SDL_Texture* texture = textures[textureID];
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	if (SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h) < 0)
	{
		PANIC("%s", SDL_GetError());
	}
	if (w > 0)
	{
		dest.w *= w;
	}
	if (h > 0)
	{
		dest.h *= h;
	}
	SDL_RenderCopy(g->rend, texture, NULL, &dest);
}

/*
	Registers a texture, returns the texture id given a filename */
TextureID Texture_RegisterTexture(char* filename)
{
	if (numTextures >= MAX_TEXTURES) 
	{
		PANIC("Texture overflow");
	}
	else
	{
		textures[numTextures] = loadTexture(filename);
		return numTextures++;
	}
}

/*
	Loads a texture using SDL2_image given a filename */
SDL_Texture* loadTexture(char* filename)
{
	SDL_Surface* surface = IMG_Load(filename);
	if (!surface) {
		PANIC("Error: %s\n", IMG_GetError());
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(g->rend, surface);
	SDL_FreeSurface(surface);
	SDL_Rect rect = { 0, 0, 64, 64 };
	SDL_RenderCopy(g->rend, texture, NULL, &rect);
	return texture;
}