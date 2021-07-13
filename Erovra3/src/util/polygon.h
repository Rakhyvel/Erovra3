#pragma once

#define MAX_VERTICES 64

typedef struct polygon_t {
    float vertexX[MAX_VERTICES];
    float vertexY[MAX_VERTICES];
    int numVertices;
} Polygon;

struct polygon_t Polygon_Create(char* filename);