#pragma once
#include <sdl.h>

void drawTexture(SDL_Texture* texture, int x, int y);
SDL_Texture* loadTexture(char* filename);