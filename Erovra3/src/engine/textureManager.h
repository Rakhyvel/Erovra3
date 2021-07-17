#pragma once
#include "../util/arraylist.h"
#include "../util/polygon.h"
#include <SDL.h>

#define MAX_TEXTURES 255
#define RAD_TO_DEG 180.0f / 3.141592653589793f
#define INVALID_TEXTURE_ID 255

typedef Uint8 TextureID;

void Texture_AlphaMod(TextureID textureID, Uint8 alpha);
void Texture_ColorMod(TextureID textureID, SDL_Color color);
void Texture_FillPolygon(TextureID textureID, Polygon polygon, SDL_Color color);
void Texture_DrawPolygon(TextureID textureID, Polygon polygon, SDL_Color color, float thickness);
void Texture_FillBezier(TextureID textureID, Polygon polygon, SDL_Color color);
void Texture_DrawBezier(TextureID textureID, Polygon polygon, SDL_Color color, float thickness);
void Texture_CreateShadow(TextureID dstID, TextureID srcID);
void Texture_Draw(TextureID texture, int x, int y, float w, float h, float angle);
TextureID Texture_RegisterTexture(char* filename);
SDL_Texture* loadTexture(char* filename);