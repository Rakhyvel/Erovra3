#pragma once
#include "textureManager.h"
#include "../util/debug.h"
#include "../util/polygon.h"
#include "../util/vector.h"
#include "apricot.h"
#include <SDL.h>
#include <SDL_image.h>
#include <float.h>
#include <stdio.h>

SDL_Texture* loadTexture(char* filename);

static SDL_Texture* textures[MAX_TEXTURES];
static int numTextures = 0;

/*
	Changes the AlphaMod of a texture given a texture id. */
void Texture_AlphaMod(TextureID textureID, Uint8 alpha)
{
    if (textureID == INVALID_TEXTURE_ID) {
        return;
    }
    SDL_Texture* texture = textures[textureID];
    SDL_SetTextureAlphaMod(texture, alpha);
}

/*
	Changes the ColorMod of a texture given a texture id. */
void Texture_ColorMod(TextureID textureID, SDL_Color color)
{
    if (textureID == INVALID_TEXTURE_ID) {
        return;
    }
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
    if (SDL_RenderCopyEx(Apricot_Renderer, texture, NULL, &dest, angle * RAD_TO_DEG, NULL, SDL_FLIP_NONE)) {
        PANIC("%s", SDL_GetError());
    }
}

/*
	Draws a texture given the texture id at a position. 
	Image width and height will be originalSize * scalar. Scalars under 0 are ignored */
void Texture_DrawCentered(TextureID textureID, int x, int y, float w, float h, float angle)
{
    if (textureID == INVALID_TEXTURE_ID) {
        return;
    }
    SDL_Texture* texture = textures[textureID];
    SDL_Rect dest;

    int textureWidth, textureHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);

    // textureWidth=width
    // textureHeight=(w/h)
    if (textureWidth > textureHeight) {
        dest.x = x;
        dest.w = w;
        dest.h = h * textureHeight / textureWidth;
        dest.y = (h - dest.h) / 2 + y;
    } else {
        dest.y = y;
        dest.w = w * textureWidth / textureHeight;
        dest.h = h;
        dest.x = (w - dest.w) / 2 + x;
    }
    if (SDL_RenderCopyEx(Apricot_Renderer, texture, NULL, &dest, angle * RAD_TO_DEG, NULL, SDL_FLIP_NONE)) {
        PANIC("%s", SDL_GetError());
    }
}

/*
	Takes in the ID of a texture, and a polygon struct and color. Draws the 
	polygon onto the texture with the given color. Texture must be created with
	target access. */
void Texture_FillPolygon(TextureID textureID, Polygon polygon, SDL_Color color)
{
    int nodes, i, j, swap;
    float nodeX[255], minY = FLT_MAX, maxY = FLT_MIN;
    SDL_Texture* texture;
    if (textureID != INVALID_TEXTURE_ID) {
        texture = textures[textureID];
    } else {
        texture = NULL;
    }

    for (i = 0; i < polygon.numVertices; i++) {
        if (polygon.vertexY[i] < minY)
            minY = polygon.vertexY[i];
        if (polygon.vertexY[i] > maxY)
            maxY = polygon.vertexY[i];
    }

    for (float pixelY = minY; pixelY < maxY + 1; pixelY++) {
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
                swap = (int)nodeX[i];
                nodeX[i] = nodeX[i + 1];
                nodeX[i + 1] = (float)swap;
                if (i)
                    i--;
            } else {
                i++;
            }
        }

        // Draw scanlines
        if (SDL_SetRenderTarget(Apricot_Renderer, texture)) {
            PANIC("%s", SDL_GetError());
        }
        SDL_SetRenderDrawColor(Apricot_Renderer, color.r, color.g, color.b, color.a);
        for (i = 0; i < nodes; i += 2) {
            if (nodeX[i] + polygon.x >= Apricot_Width)
                break;
            if (nodeX[i + 1] > 0) {
                if (nodeX[i] + polygon.x < 0)
                    nodeX[i] = 0;
                if (nodeX[i + 1] + polygon.x > Apricot_Width)
                    nodeX[i + 1] = (float)Apricot_Width;
                SDL_RenderDrawLine(Apricot_Renderer, (int)nodeX[i] + polygon.x, (int)pixelY - 1 + polygon.y, (int)nodeX[i + 1] + polygon.x, (int)pixelY - 1 + polygon.y);
            }
        }
        SDL_SetRenderTarget(Apricot_Renderer, NULL);
    }
}

void drawCircle(TextureID textureID, Vector center, float radius, SDL_Color color)
{
    SDL_Texture* texture;
    if (textureID != INVALID_TEXTURE_ID) {
        texture = textures[textureID];
    } else {
        texture = NULL;
    }
    if (SDL_SetRenderTarget(Apricot_Renderer, texture)) {
        PANIC("%s", SDL_GetError());
    }
    SDL_SetRenderDrawColor(Apricot_Renderer, color.r, color.g, color.b, color.a);
    for (double y = -radius; y < radius; y++) {
        double newY = y + center.y;
        double x = radius * (double)sqrtf(1.0 - powf(y / (double)radius, 2.0));
        SDL_RenderDrawLine(Apricot_Renderer, max(-x + center.x, -1), y + center.y, x + center.x, y + center.y);
    }
    SDL_SetRenderTarget(Apricot_Renderer, NULL);
}

/*
	Takes two points, and a thickness. Creates a polygon representing a rectangle
	with the thickness that spans between p1 and p2. */
void drawThickLine(TextureID textureID, Vector p1, Vector p2, SDL_Color color, float thickness)
{
    if (fabs(p1.x - p2.x) < 0.001) {
        p1.x += 0.001f;
    }
    if (fabs(p1.y - p2.y) < 0.001) {
        p1.y += 0.001f;
    }
    Polygon rectangle;
    rectangle.numVertices = 4;
    rectangle.x = 0;
    rectangle.y = 0;
    float slope = (p1.y - p2.y) / (p1.x - p2.x);
    float slopeSquared = slope * slope;
    float inverseSlope = -1.0f / slope;
    float circleOffset = thickness * sqrtf(slopeSquared / (slopeSquared + 1));

    rectangle.vertexX[0] = p1.x + circleOffset;
    rectangle.vertexX[1] = p1.x - circleOffset;
    rectangle.vertexX[2] = p2.x - circleOffset;
    rectangle.vertexX[3] = p2.x + circleOffset;

    rectangle.vertexY[0] = inverseSlope * circleOffset + p1.y;
    rectangle.vertexY[1] = inverseSlope * -circleOffset + p1.y;
    rectangle.vertexY[2] = inverseSlope * -circleOffset + p2.y;
    rectangle.vertexY[3] = inverseSlope * circleOffset + p2.y;

    Texture_FillPolygon(textureID, rectangle, color);
}

/*
	Goes through the points in a polygon. Draws a line between each point */
void Texture_DrawPolygon(TextureID textureID, Polygon polygon, SDL_Color color, float thickness)
{
    // For each edge in the polygon
    for (int i = 0; i < polygon.numVertices; i++) {
        // Draw a line between the vertex i and i+1
        Vector p1 = { polygon.vertexX[i] + polygon.x, polygon.vertexY[i] + polygon.y };
        Vector p2 = { polygon.vertexX[(i + 1) % polygon.numVertices] + polygon.x, polygon.vertexY[(i + 1) % polygon.numVertices] + polygon.y };
        drawThickLine(textureID, p1, p2, color, thickness);
        drawCircle(textureID, p1, thickness, color);
    }
}

/*
	Takes in an array of x values and an array of y values where the first and 
	fourth elements are fixed points, and the second and third are slope points.
	
	Returns the vector <x,y> of the point along the curve from 0-1, with 0 being
	closer to the first point and 1 being closer to the fourth and last point.*/
static Vector bezierCurve(float x[4], float y[4], double u)
{
    float xu = 0.0f, yu = 0.0f;
    xu = (float)(pow(1 - u, 3) * x[0] + 3 * u * pow(1 - u, 2) * x[1] + 3 * pow(u, 2) * (1 - u) * x[2]
        + pow(u, 3) * x[3]);
    yu = (float)(pow(1 - u, 3) * y[0] + 3 * u * pow(1 - u, 2) * y[1] + 3 * pow(u, 2) * (1 - u) * y[2]
        + pow(u, 3) * y[3]);
    return (Vector) { xu, yu };
}

/*
	Interprets the polygon's vertices as control points for a closed bezier curve.
	Approximates the bezier curve with more vertices.*/
static Polygon spliceBezier(Polygon polygon)
{
    float tempX[4];
    float tempY[4];
    Polygon rendval;
    rendval.numVertices = 0;
    int rendVertexIndex = 0;
    const float detail = 16.0f;
    for (int i = 0; i < polygon.numVertices; i += 3) {
        // Fill temp with the four points from the polygon
        for (int j = 0; j < 4; j++) {
            tempX[j] = polygon.vertexX[(i + j) % polygon.numVertices];
            tempY[j] = polygon.vertexY[(i + j) % polygon.numVertices];
        }
        // Add new vertices to the polygon based on a time step
        for (int j = 0; j < detail; j++, rendVertexIndex++) {
            Vector vertex = bezierCurve(tempX, tempY, (float)j / detail);
            rendval.vertexX[rendVertexIndex] = vertex.x;
            rendval.vertexY[rendVertexIndex] = vertex.y;
            rendval.numVertices++;
        }
    }
    return rendval;
}

/*
	Fills the spliced version of a bezier */
void Texture_FillBezier(TextureID textureID, Polygon polygon, SDL_Color color)
{
    Polygon splicedBezier = spliceBezier(polygon);
    splicedBezier.x = polygon.x;
    splicedBezier.y = polygon.y;
    Texture_FillPolygon(textureID, splicedBezier, color);
}

/*
	Draws the spliced version of a bezier polygon */
void Texture_DrawBezier(TextureID textureID, Polygon polygon, SDL_Color color, float thickness)
{
    Polygon splicedBezier = spliceBezier(polygon);
    splicedBezier.x = polygon.x;
    splicedBezier.y = polygon.y;
    Texture_DrawPolygon(textureID, splicedBezier, color, thickness);
}

/*
	Draws a shadow on the destination texture if for each corresponding pixel in 
	the source texture is non-transparent */
void Texture_CreateShadow(TextureID dstID, TextureID srcID)
{
    SDL_Texture* src = textures[srcID];
    SDL_Texture* dst = textures[dstID];
    int srcFormat = SDL_PIXELFORMAT_ARGB8888, srcPitch = 4, w, h;
    SDL_QueryTexture(src, &srcFormat, NULL, &w, &h);
    Uint8* srcPixels = calloc(srcPitch * w * h, 1);
    if (!srcPixels) {
        PANIC("Mem err");
    }

    // Set src texture as target, get array of pixels
    SDL_SetRenderTarget(Apricot_Renderer, src);
    SDL_RenderCopy(Apricot_Renderer, src, NULL, NULL);
    SDL_RenderReadPixels(Apricot_Renderer, NULL, srcFormat, srcPixels, srcPitch * w);

    // Set dst texture as target, if src pixel non-transparent, dst pixel is shadow colored
    SDL_SetRenderTarget(Apricot_Renderer, dst);
    SDL_SetRenderDrawColor(Apricot_Renderer, 0, 0, 0, 0);
    SDL_RenderClear(Apricot_Renderer);
    SDL_SetRenderDrawColor(Apricot_Renderer, 0, 0, 0, 64);
    int area = 0;
    for (int i = 0; i < w * h * 4; i += 4) {
        int x = (i / 4) % w;
        int y = (i / 4) / w;
        if (srcPixels[i] || srcPixels[i + 1] || srcPixels[i + 2] || srcPixels[i + 3]) {
            SDL_RenderDrawPoint(Apricot_Renderer, x, y);
            area++;
        }
    }
    SDL_SetRenderTarget(Apricot_Renderer, NULL);
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
    SDL_Texture* imgTexture = IMG_LoadTexture(Apricot_Renderer, filename);
    if (!imgTexture) {
        PANIC("Error: %s\n", IMG_GetError());
    }

    // Create a new texture that can be rendered on
    SDL_Rect rect = { 0, 0, 0, 0 };
    SDL_QueryTexture(imgTexture, 0, 0, &rect.w, &rect.h);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_Texture* accessibleTexture = SDL_CreateTexture(Apricot_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, rect.w, rect.h);

    // Copy IMG texture to drawible texture
    SDL_SetRenderTarget(Apricot_Renderer, accessibleTexture);
    SDL_SetTextureBlendMode(accessibleTexture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(Apricot_Renderer, imgTexture, NULL, &rect);
    SDL_SetRenderTarget(Apricot_Renderer, NULL);
    SDL_DestroyTexture(imgTexture);

    return accessibleTexture;
}