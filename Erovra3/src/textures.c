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
    PORT_TEXTURE_ID = Texture_RegisterTexture("res/port.png");
    BUILDING_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/hit.png");
    BUILDING_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/building_shadow.png");
    CAPITAL_TEXTURE_ID = Texture_RegisterTexture("res/capital.png");
    WALL_TEXTURE_ID = Texture_RegisterTexture("res/wall.png");
    WALL_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/wall_shadow.png");

    INFANTRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    CAVALRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    ARTILLERY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");
    GROUND_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/ground_outline.png");
    GROUND_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/shadow.png");

    BATTLESHIP_TEXTURE_ID = Texture_RegisterTexture("res/ship.png");
    SHIP_SHADOW_TEXTURE_ID = Texture_RegisterTexture("res/ship.png");
    SHIP_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/ship.png");

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

    Texture_DrawPolygon(BATTLESHIP_TEXTURE_ID, Polygon_Create("res/ship_fill.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(BATTLESHIP_TEXTURE_ID, Polygon_Create("res/ship_border.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_DrawPolygon(BATTLESHIP_TEXTURE_ID, Polygon_Create("res/cruiser_stripe.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_DrawPolygon(SHIP_OUTLINE_TEXTURE_ID, Polygon_Create("res/ship_fill.gon"), (SDL_Color) { 255, 255, 255, 255 });
    Texture_DrawPolygon(SHIP_SHADOW_TEXTURE_ID, Polygon_Create("res/ship_fill.gon"), (SDL_Color) { 0, 0, 0, 60 });
}