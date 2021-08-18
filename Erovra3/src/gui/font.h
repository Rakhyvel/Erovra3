#pragma once
#include <SDL.h>

extern SDL_Texture* font;

extern int kern16[];

void Font_Init();

int Font_GetWidth(char* str);
int Font_GetSubStringWidth(char* str, int i);
int Font_GetCharIndex(char* str, int x);
void Font_DrawString(char* str, int x, int y);