/*	textureManager.h
* 
*	Provides implementation for loading, creating, and drawing on textures.
* 
*	@author	Joseph Shimel
*	@date	4/6/21
*/

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "../util/polygon.h"
#include "../util/vector.h"
#include <SDL.h>

#define RAD_TO_DEG 180.0f / 3.141592653589793f

/**
 * @brief Draws a texture to the screen
 * @param texture Texture of the texture to draw
 * @param x X coordinate relative to the top-left corner of the screen
 * @param y Y coordinate relative to the top-left corner of the screen
 * @param w Width in pixels
 * @param h Height in pixels
 * @param angle Rotation angle in degrees
*/
void Texture_Draw(SDL_Texture* texture, int x, int y, float w, float h, float angle);

/**
 * @brief Draws a texture to the screen, centered at the position
 * @param texture Texture of the texture to draw
 * @param x X coordinate relative to the top-left corner of the screen
 * @param y Y coordinate relative to the top-left corner of the screen
 * @param w Width in pixels
 * @param h Height in pixels
 * @param angle Rotation angle in degrees
*/
void Texture_DrawCentered(SDL_Texture* texture, int x, int y, float w, float h, float angle);

/**
 * @brief Fills the polygon onto the texture with the given color
 * @param texture Texture to draw the polygon onto. Underlying texture must have target access 
 * @param polygon Polygon to fill
 * @param color Color to fill
*/
void Texture_FillPolygon(SDL_Texture* texture, Polygon polygon, SDL_Color color);

/**
 * @brief Fills a circle onto a texture
 * @param texture Texture to fill circle onto
 * @param center Center position of circle, relative to screen
 * @param radius Radius of circle in pixels
 * @param color Color of circle
*/
void Texture_FillCircle(SDL_Texture* texture, Vector center, float radius, SDL_Color color);

/**
 * @brief Draws a line with thickness (rectangle) between two points
 * @param texture Texture to draw thick line to
 * @param p1 First point, relative to screen
 * @param p2 Second point, relative to screen
 * @param color Color of line
 * @param thickness Thickness of line, from edge to center of line, in pixels
*/
void Texture_DrawThickLine(SDL_Texture* texture, Vector p1, Vector p2, SDL_Color color, float thickness);

/**
 * @brief Draws the outline of a polygon
 * @param texture Texture to draw the polygon onto
 * @param polygon The polygon to draw
 * @param color The color of the polygon
 * @param thickness Thickness of the outline, from edge of line to center, in pixels
*/
void Texture_DrawPolygon(SDL_Texture* texture, Polygon polygon, SDL_Color color, float thickness);

/**
 * @brief Fills in a closed bezier shape
 * @param texture Texture to draw bezier shape to
 * @param polygon Polygon representing a closed bezier shape (see util/polygon.h for details)
 * @param color Color of bezier shape
*/
void Texture_FillBezier(SDL_Texture* texture, Polygon polygon, SDL_Color color);

/**
 * @brief Draws the outline of a closed bezier shape
 * @param texture Texture to draw onto
 * @param polygon Polygon representing a closed bezier shape (see util/polygon.h for details)
 * @param color Color of outline
 * @param thickness Thickness of outline, from edge of outline to center, in pixels
*/
void Texture_DrawBezier(SDL_Texture* texture, Polygon polygon, SDL_Color color, float thickness);

/**
 * @brief Draws a shadow for all non-transparent piels in the source texture
 * @param dst Texture to draw the shadow onto
 * @param src Texture to create shadow from
*/
void Texture_CreateShadow(SDL_Texture* dst, SDL_Texture* src);

/**
 * @brief Loads an SDL_Texture from a file
 * @param filename Path to the texture
 * @return An SDL_Texture with target access
*/
SDL_Texture* Texture_Load(char* filename);

/**
 * @brief Creates a blank SDL_Texture given a width and a height
 * @param width Width of texture in pixels
 * @param height Height of texture in pixels
 * @return A blank SDL_Texture with target access, of the size specified
*/
SDL_Texture* Texture_Create(int width, int height);

/**
 * @brief Paints a map depending on a color function given a height
 * @param map A height map to paint the texture based on
 * @param mapSize The width and height of both the height map and the texture
 * @param texture The texture to paint
 * @param colorFunction The function that takes in the map, mapSize, position, and the height on the map and returns a color for that pixel.
*/
void Texture_PaintMap(float* map, int mapSize, SDL_Texture* texture, SDL_Color(colorFunction)(float* map, int mapSize, int x, int y, float i));

void Texture_Save(const char* filename, SDL_Texture* texture);

#endif