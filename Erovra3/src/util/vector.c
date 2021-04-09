#pragma once
#include "vector.h"
#include <math.h>

/*
	Finds the euclidian distance between two vectors */
float Vector_Dist(struct vector* a, struct vector* b)
{
    return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
}