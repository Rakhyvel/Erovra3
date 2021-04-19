#pragma once
#include "font.h"
#include "../engine/gameState.h"
#include "../engine/textureManager.h"

SDL_Texture* font;

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
    font = loadTexture("res/font16.png");
}

/*
	Gets the on-screen width of text */
int Font_GetWidth(char* str)
{
    int retval = 0;
    int i = 0;
    while (str[i] != '\0') {
        retval += kern16[str[i]] + 1;
        i++;
    }
    return retval;
}

/*
	Draws a string to the screen */
void Font_DrawString(char* str, int x, int y)
{
    SDL_SetTextureColorMod(font, 255, 255, 255, 255);
    int i = 0;
    SDL_Rect src = {0, 0, 16, 16};
    SDL_Rect dest = { x, y, 16, 16 };
    while (str[i] != '\0') {
        src.x = (str[i] % 16) * 16;
        src.y = (int)(str[i] / 16) * 16;
        SDL_RenderCopy(g->rend, font, &src, &dest);
        dest.x += kern16[str[i]] + 1;
        i++;
    }
}