#pragma once
#include "./textures.h"
#include "./util/polygon.h"

void Textures_Init()
{
    CITY_TEXTURE_ID = Texture_RegisterTexture("res/city.png");
    BUILDING_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/hit.png");
    CAPITAL_TEXTURE_ID = Texture_RegisterTexture("res/capital.png");
    INFANTRY_TEXTURE_ID = Texture_RegisterTexture("res/infantry.png");

    float polyX[] = { 0.0, 320.0, 320.0, 0.0, 0.0, 20.0, 20.0, 300.0, 300.0, 0.0 };
    float polyY[] = { 0.0, 0.0, 160.0, 160.0, 20.0, 20.0, 140.0, 140.0, 20.0, 20.0 };
    Texture_DrawPolygon(INFANTRY_TEXTURE_ID, Polygon_Create("res/ground_border.gon"), (SDL_Color) { 0, 0, 0, 255 });
    Texture_DrawPolygon(INFANTRY_TEXTURE_ID, Polygon_Create("res/infantry.gon"), (SDL_Color) { 0, 0, 0, 255 });
}