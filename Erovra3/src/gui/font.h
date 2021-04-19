#pragma once
#include <SDL.h>

extern SDL_Texture* font;

extern int kern16[];

void Font_Init();

int Font_GetWidth(char* str);
void Font_DrawString(char* str, int x, int y);