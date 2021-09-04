#pragma once
#include "font.h"
#include "../engine/gameState.h"
#include "../engine/textureManager.h"
#include "../util/debug.h"
#include "SDL_FontCache.h"
#include <string.h>

FC_Font* font = NULL;
FC_Font* bigFont = NULL;

int kern16[] = {
    0, 0, 4, 4, 4, 1, 7, 16, 7, 0, 0, 6, 5, 0, 6, 5,
    7, 6, 5, 4, 5, 7, 7, 4, 5, 4, 6, 6, 0, 0, 0, 0,
    8, 3, 5, 9, 7, 12, 9, 2, 4, 4, 5, 8, 2, 4, 2, 6,
    7, 5, 7, 7, 8, 7, 7, 7, 7, 7, 2, 2, 8, 8, 8, 7,
    13, 11, 9, 9, 9, 9, 8, 10, 9, 3, 7, 9, 7, 11, 9, 10,
    9, 10, 9, 9, 9, 9, 10, 13, 9, 10, 7, 4, 5, 4, 5, 9,
    3, 7, 7, 7, 7, 7, 6, 7, 7, 3, 4, 7, 3, 11, 7, 7,
    8, 7, 5, 7, 5, 7, 7, 11, 7, 7, 8, 5, 3, 5, 8, 4
};

/*
	Inits the font spritsheet */
void Font_Init()
{
    if (!font) {
        font = FC_CreateFont();
        bigFont = FC_CreateFont();
        FC_LoadFont(font, g->rend, "res/gui/Segoe UI.ttf", 16, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
        FC_LoadFont(bigFont, g->rend, "res/gui/Segoe UI.ttf", 175, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
    }
}

/*
	Gets the on-screen width of text */
int Font_GetWidth(char* str)
{
    return FC_GetWidth(font, str);
}

int Font_GetSubStringWidth(char* str, int length)
{
    char buffer[255];
    memset(buffer, 0, 255);
    if (length >= 255) {
        PANIC("Um lol?");
    }
    for (int i = 0; i < length; i++) {
        buffer[i] = str[i];
    }
    return FC_GetWidth(font, buffer);
}

int Font_GetCharIndex(char* str, int x)
{
    int total = 0;
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        total += kern16[str[i]] + 1;
        if (total > x + (kern16[str[i]] + 1) / 2) {
            return i;
        }
    }
    return i;
}

/*
	Draws a string to the screen */
void Font_DrawString(char* str, int x, int y)
{
    FC_Draw(font, g->rend, x, y - 4, str);
    /*
    SDL_SetTextureColorMod(font, 255, 255, 255);
    int i = 0;
    SDL_Rect src = { 0, 0, 16, 16 };
    SDL_Rect dest = { x, y, 16, 16 };
    while (str[i] != '\0') {
        src.x = (str[i] % 16) * 16;
        src.y = (int)(str[i] / 16) * 16;
        SDL_RenderCopy(g->rend, font, &src, &dest);
        dest.x += kern16[str[i]] + 1;
        i++;
    }
	*/
}