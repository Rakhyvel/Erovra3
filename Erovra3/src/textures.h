#pragma once
#include "engine/textureManager.h"

/*
In this file, I list out all the textures IDs used. They are then initialized in textures.c

This is pretty dumb. Lots of things are. But this is like really bad
*/

TextureID CITY_TEXTURE_ID;
TextureID BUILDING_OUTLINE_TEXTURE_ID;
TextureID CAPITAL_TEXTURE_ID;

void Textures_Init();