#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <float.h>
#include <stdio.h>

#include "../util/debug.h"
#include "../util/polygon.h"
#include "gameState.h"
#include "textureManager.h"

SDL_Texture* loadTexture(char* filename);

static SDL_Texture* textures[MAX_TEXTURES];
static int numTextures = 0;

void Texture_AlphaMod(TextureID textureID, Uint8 alpha)
{
    SDL_Texture* texture = textures[textureID];
    SDL_SetTextureAlphaMod(texture, alpha);
}

/*
	Changes the ColorMod of a texture given a texture id. */
void Texture_ColorMod(TextureID textureID, SDL_Color color)
{
    SDL_Texture* texture = textures[textureID];
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
}

/*
	Draws a texture given the texture id at a position. 
	Image width and height will be originalSize * scalar. Scalars under 0 are ignored */
void Texture_Draw(TextureID textureID, int x, int y, float w, float h, float angle)
{
    if (textureID == INVALID_TEXTURE_ID) {
        return;
    }
    SDL_Texture* texture = textures[textureID];
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = w;
    dest.h = h;
    if (SDL_RenderCopyEx(g->rend, texture, NULL, &dest, angle * RAD_TO_DEG, NULL, SDL_FLIP_NONE)) {
        PANIC("%s", SDL_GetError());
    }
}

/*
	Takes in the ID of a texture, and a polygon struct and color. Draws the 
	polygon onto the texture with the given color. Texture must be created with
	target access. */
void Texture_DrawPolygon(TextureID textureID, Polygon polygon, SDL_Color color)
{
    int nodes, pixelX, pixelY, i, j, swap;
    float nodeX[255], minY = FLT_MAX, maxY = 0;
    SDL_Texture* texture = textures[textureID];

    for (i = 0; i < polygon.numVertices; i++) {
        if (polygon.vertexY[i] < minY)
            minY = polygon.vertexY[i];
        if (polygon.vertexY[i] > maxY)
            maxY = polygon.vertexY[i];
    }

    for (float pixelY = (int)minY; pixelY < maxY + 1; pixelY++) {
        nodes = 0;
        j = polygon.numVertices - 1;
        // Build list of the x coords for each intersection
        for (i = 0; i < polygon.numVertices; i++) {
            if (polygon.vertexY[i] < (float)pixelY && polygon.vertexY[j] >= (float)pixelY
                || polygon.vertexY[j] < (float)pixelY && polygon.vertexY[i] >= (float)pixelY) {
                nodeX[nodes++] = (polygon.vertexX[i] + (pixelY - polygon.vertexY[i]) / (polygon.vertexY[j] - polygon.vertexY[i]) * (polygon.vertexX[j] - polygon.vertexX[i]));
            }
            j = i;
        }

        // Bubble sort x coords, so that you can run through and draw scanlines
        i = 0;
        while (i < nodes - 1) {
            if (nodeX[i] > nodeX[i + 1]) {
                swap = nodeX[i];
                nodeX[i] = nodeX[i + 1];
                nodeX[i + 1] = swap;
                if (i)
                    i--;
            } else {
                i++;
            }
        }

        // Draw scanlines
        if (SDL_SetRenderTarget(g->rend, texture)) {
            PANIC("%s", SDL_GetError());
        }
        SDL_SetRenderDrawColor(g->rend, color.r, color.g, color.b, color.a);
        for (i = 0; i < nodes; i += 2) {
            if (nodeX[i] >= g->width)
                break;
            if (nodeX[i + 1] > 0) {
                if (nodeX[i] < 0)
                    nodeX[i] = 0;
                if (nodeX[i + 1] > g->width)
                    nodeX[i + 1] = g->width;
                SDL_RenderDrawLine(g->rend, nodeX[i], pixelY - 1, nodeX[i + 1], pixelY - 1);
            }
        }
        SDL_SetRenderTarget(g->rend, NULL);
    }
}

/*
	Registers a texture, returns the texture id given a filename */
TextureID Texture_RegisterTexture(char* filename)
{
    if (numTextures >= MAX_TEXTURES) {
        PANIC("Texture overflow");
    } else {
        textures[numTextures] = loadTexture(filename);
        return numTextures++;
    }
}

/*
	Loads a texture using SDL2_image given a filename */
SDL_Texture* loadTexture(char* filename)
{
    // Create texture from img, which cannot be rendered on
    SDL_Texture* imgTexture = IMG_LoadTexture(g->rend, filename);
    if (!imgTexture) {
        PANIC("Error: %s\n", IMG_GetError());
    }

    // Create a new texture that can be rendered on
    SDL_Rect rect = { 0, 0, 0, 0 };
    SDL_QueryTexture(imgTexture, 0, 0, &rect.w, &rect.h);
    if (rect.h > 32) {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    } else {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    }
    SDL_Texture* accessibleTexture = SDL_CreateTexture(g->rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, rect.w, rect.h);

    // Copy IMG texture to drawible texture
    SDL_SetRenderTarget(g->rend, accessibleTexture);
    SDL_SetTextureBlendMode(accessibleTexture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(g->rend, imgTexture, NULL, &rect);
    SDL_SetRenderTarget(g->rend, NULL);
    SDL_DestroyTexture(imgTexture);

    return accessibleTexture;
}