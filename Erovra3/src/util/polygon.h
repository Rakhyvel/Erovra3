/** 
* # BEZIER SHAPES
*/

#pragma once

#define MAX_VERTICES 128

typedef struct polygon_t {
    int x, y;
    float vertexX[MAX_VERTICES];
    float vertexY[MAX_VERTICES];
    int numVertices;
} Polygon;

struct polygon_t Polygon_Create(char* filename);