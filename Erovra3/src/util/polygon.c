#pragma once
#include "polygon.h"
#include "debug.h"
#include <stdio.h>

/*
	Takes in a filename to a .gon file. The file should have vertices on each new 
	line, with floats separated by whitespace for the coordinates. For example:
		0.0		0.0
		100.0	0.0
		100.0	100.0
		0.0		100.0
	
	Returns a Polygon struct that contains the data of the polygon */
struct polygon_t Polygon_Create(char* filename)
{
    FILE* gonFile;

    fopen_s(&gonFile, filename, "r");
    if (!gonFile) {
        perror(filename);
        PANIC("Pausing...");
    }

    struct polygon_t retval;
    int code, numVertices = 0;
    float x, y;
    while ((code = fscanf_s(gonFile, "%f", &x)) != EOF) {
        if (numVertices > MAX_VERTICES) {
            PANIC("%s has too many vertices specified (>%d)", filename, MAX_VERTICES);
        }
        retval.vertexX[numVertices] = x;

        if (fscanf_s(gonFile, "%f", &y) == EOF) {
            PANIC("Format error in %s", filename);
        }
        retval.vertexY[numVertices] = y;
        numVertices++;
    }
    retval.numVertices = numVertices;
    fclose(gonFile);
    return retval;
}
