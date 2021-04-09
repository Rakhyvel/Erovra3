#pragma once
#include "./textures.h"
#include "./util/polygon.h"

/*
	Initializes TextureID's used in game, draws polygons onto some of them */
void Textures_Init()
{
    CITY_TEXTURE_ID = Texture_RegisterTexture("res/city.png");
    BUILDING_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/hit.png");
    CAPITAL_TEXTURE_ID = Texture_RegisterTexture("res/capital.png");
    INFANTRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    GROUND_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/ground_outline.png");

    Texture_DrawPolygon(INFANTRY_TEXTURE_ID, Polygon_Create("res/ground_border.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_DrawPolygon(INFANTRY_TEXTURE_ID, Polygon_Create("res/infantry.gon"), (SDL_Color) { 0, 0, 0, 255 });
}