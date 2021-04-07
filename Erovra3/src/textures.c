#pragma once
#include "./textures.h"

void Textures_Init()
{
    CITY_TEXTURE_ID = Texture_RegisterTexture("res/city.png");
    BUILDING_OUTLINE_TEXTURE_ID = Texture_RegisterTexture("res/hit.png");
    CAPITAL_TEXTURE_ID = Texture_RegisterTexture("res/capital.png");
}