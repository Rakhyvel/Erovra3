#pragma once
#include <SDL.h>
#include "../util/arraylist.h"

#define MAX_TEXTURES 255

typedef Uint8 TextureID;

void Texture_ColorMod(TextureID textureID, SDL_Color color);
void Texture_Draw(TextureID texture, int x, int y, float w, float h);
TextureID Texture_RegisterTexture(char* filename);