#include "textureManager.h"
#include "../util/debug.h"
#include "../util/polygon.h"
#include "../util/vector.h"
#include "apricot.h"
#include <SDL.h>
#include <SDL_image.h>
#include <float.h>
#include <stdio.h>

/**
 * @brief Calculates the parametric curve based on 4 bezier control points
 * @param x Array of x values for control points
 * @param y Array of y values for control points
 * @param u Parametric variable in the range [0-1]
 * @return The position along the curve for the value of u
*/
static Vector bezierCurve(float x[4], float y[4], double u)
{
    float xu = 0.0f, yu = 0.0f;
    xu = (int)(pow(1 - u, 3) * x[0] + 3 * u * pow(1 - u, 2) * x[1] + 3 * pow(u, 2) * (1 - u) * x[2]
        + pow(u, 3) * x[3]);
    yu = (int)(pow(1 - u, 3) * y[0] + 3 * u * pow(1 - u, 2) * y[1] + 3 * pow(u, 2) * (1 - u) * y[2]
        + pow(u, 3) * y[3]);
    return (Vector) { xu, yu };
}

/**
 * @brief Interprets a polygon's vertices as control points for a closed bezier curve. Approximates the bezier curve with more vertices
 * @param polygon Polygon representing the closed bezier curve
 * @return A rigid polygon based on the bezier curve
*/
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

void Texture_Draw(SDL_Texture* texture, int x, int y, float w, float h, float angle)
{
    SDL_Rect dest = { x, y, w, h };
    if (SDL_RenderCopyEx(Apricot_Renderer, texture, NULL, &dest, angle * RAD_TO_DEG, NULL, SDL_FLIP_NONE)) {
        PANIC("%s", SDL_GetError());
    }
}

void Texture_DrawCentered(SDL_Texture* texture, int x, int y, float w, float h, float angle)
{
    SDL_Rect dest;
    int textureWidth;
    int textureHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);
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

void Texture_FillPolygon(SDL_Texture* texture, Polygon polygon, SDL_Color color)
{
    int nodes, i, j, swap;
    float nodeX[255], minY = FLT_MAX, maxY = FLT_MIN;

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

void Texture_FillCircle(SDL_Texture* texture, Vector center, float radius, SDL_Color color)
{
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

void Texture_DrawThickLine(SDL_Texture* texture, Vector p1, Vector p2, SDL_Color color, float thickness)
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

    Texture_FillPolygon(texture, rectangle, color);
}

void Texture_DrawPolygon(SDL_Texture* texture, Polygon polygon, SDL_Color color, float thickness)
{
    // For each edge in the polygon
    for (int i = 0; i < polygon.numVertices; i++) {
        // Draw a line between the vertex i and i+1
        Vector p1 = { polygon.vertexX[i] + polygon.x, polygon.vertexY[i] + polygon.y };
        Vector p2 = { polygon.vertexX[(i + 1) % polygon.numVertices] + polygon.x, polygon.vertexY[(i + 1) % polygon.numVertices] + polygon.y };
        Texture_DrawThickLine(texture, p1, p2, color, thickness);
        Texture_FillCircle(texture, p1, thickness, color);
    }
}

void Texture_FillBezier(SDL_Texture* texture, Polygon polygon, SDL_Color color)
{
    Polygon splicedBezier = spliceBezier(polygon);
    splicedBezier.x = polygon.x;
    splicedBezier.y = polygon.y;
    Texture_FillPolygon(texture, splicedBezier, color);
}

void Texture_DrawBezier(SDL_Texture* texture, Polygon polygon, SDL_Color color, float thickness)
{
    Polygon splicedBezier = spliceBezier(polygon);
    splicedBezier.x = polygon.x;
    splicedBezier.y = polygon.y;
    Texture_DrawPolygon(texture, splicedBezier, color, thickness);
}

void Texture_CreateShadow(SDL_Texture* dst, SDL_Texture* src)
{
    int srcFormat = SDL_PIXELFORMAT_ARGB8888;
    int srcPitch = 4;
    int w;
    int h;
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

SDL_Texture* Texture_Load(char* filename)
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

SDL_Texture* Texture_Create(int width, int height)
{
    SDL_Texture* texture = SDL_CreateTexture(Apricot_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);

    // Clear texture so that it is completely transparent
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(Apricot_Renderer, texture);
    SDL_SetRenderDrawColor(Apricot_Renderer, 0, 0, 0, 0);
    SDL_RenderClear(Apricot_Renderer);
    SDL_SetRenderTarget(Apricot_Renderer, NULL);

    return texture;
}

void Texture_PaintMap(float* map, int mapSize, SDL_Texture* texture, SDL_Color(colorFunction)(float* map, int mapSize, int x, int y, float i))
{
    Uint8* pixels = calloc(mapSize * mapSize, 4);
    if (!pixels) {
        PANIC("Memory error Texture_PaintMap() creating pixels\n");
    }
    for (int x = 0; x < mapSize; x++) {
        for (int y = 0; y < mapSize; y++) {
            const unsigned int offset = (mapSize * 4 * y) + x * 4;
            float i = map[y * mapSize + x];

            SDL_Color terrainColor = colorFunction(map, mapSize, x, y, i);
            pixels[offset + 0] = terrainColor.b;
            pixels[offset + 1] = terrainColor.g;
            pixels[offset + 2] = terrainColor.r;
            pixels[offset + 3] = SDL_ALPHA_OPAQUE;
        }
    }
    if (SDL_UpdateTexture(texture, NULL, pixels, mapSize * 4) == -1) {
        PANIC("%s", SDL_GetError());
    }
    free(pixels);
}