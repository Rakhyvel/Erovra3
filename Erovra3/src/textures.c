#pragma once
#include "./textures.h"
#include "./util/polygon.h"

/*
	Initializes TextureID's used in game, draws polygons onto some of them */
void Textures_Init()
{
    CITY_TEXTURE_ID = Texture_RegisterTexture("res/city.png");
    FACTORY_TEXTURE_ID = Texture_RegisterTexture("res/factory.png");
    MINE_TEXTURE_ID = Texture_RegisterTexture("res/mine.png");
    BUILDING_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/hit.png");
    BUILDING_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/building_shadow.png");
    CAPITAL_TEXTURE_ID = Texture_RegisterTexture("res/capital.png");
    INFANTRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    CAVALRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    ARTILLERY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    GROUND_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/ground_outline.png");
    GROUND_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/shadow.png");
    BULLET_TEXTURE_ID = Texture_RegisterTexture("res/bullet.png");
    SHELL_TEXTURE_ID = Texture_RegisterTexture("res/shell.png");
    COIN_TEXTURE_ID = Texture_RegisterTexture("res/coin.png");
    ORE_TEXTURE_ID = Texture_RegisterTexture("res/ore.png");

    Texture_DrawPolygon(INFANTRY_TEXTURE_ID, Polygon_Create("res/ground_border.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_DrawPolygon(INFANTRY_TEXTURE_ID, Polygon_Create("res/infantry.gon"), (SDL_Color) { 0, 0, 0, 255 });

    Texture_DrawPolygon(CAVALRY_TEXTURE_ID, Polygon_Create("res/ground_border.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_DrawPolygon(CAVALRY_TEXTURE_ID, Polygon_Create("res/cavalry.gon"), (SDL_Color) { 0, 0, 0, 255 });

    Texture_DrawPolygon(ARTILLERY_TEXTURE_ID, Polygon_Create("res/ground_border.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_DrawPolygon(ARTILLERY_TEXTURE_ID, Polygon_Create("res/artillery.gon"), (SDL_Color) { 0, 0, 0, 255 });
}