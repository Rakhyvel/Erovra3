#pragma once
#include "vector.h"
#include <math.h>

struct vector Vector_Normalize(struct vector a)
{
    float mag = Vector_Magnitude(a);
    if (mag == 0) {
        return;
    } else {
        return Vector_Scalar(a, 1.0f / mag);
    }
}

struct vector Vector_Add(struct vector a, struct vector b)
{
    return (struct vector) { a.x + b.x, a.y + b.y };
}

struct vector Vector_Sub(struct vector a, struct vector b)
{
    return (struct vector) { a.x - b.x, a.y - b.y };
}

struct vector Vector_Scalar(struct vector a, float scalar)
{
    return (struct vector) { a.x * scalar, a.y * scalar };
}

/*
	Finds the euclidian distance between two vectors */
float Vector_Dist(struct vector a, struct vector b)
{
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float Vector_Magnitude(struct vector a)
{
    return sqrtf(a.x * a.x + a.y * a.y);
}

float Vector_Angle(struct vector a)
{
    if (a.y != 0) {
        return -atan2(a.x, a.y);
    } else if (a.x == 0) {
        return 0;
    } else if (a.x > 0) {
        return -1.5707963267948965;
    } else if (a.x < 0) {
        return 1.5707963267948965;
    }
}
