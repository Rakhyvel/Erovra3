#pragma once
#include "vector.h"
#include <math.h>

/*
	Returns a vector in the same direction with the magnitude of 1 */
struct vector Vector_Normalize(struct vector a)
{
    float mag = Vector_Magnitude(a);
    if (mag == 0) {
        return (Vector) { 0, 0 };
    } else {
        return Vector_Scalar(a, 1.0f / mag);
    }
}

/*
	Returns the sum of two vectors */
struct vector Vector_Add(struct vector a, struct vector b)
{
    return (struct vector) { a.x + b.x, a.y + b.y };
}

/*
	Returns the difference between two vectors */
struct vector Vector_Sub(struct vector a, struct vector b)
{
    return (struct vector) { a.x - b.x, a.y - b.y };
}

/*
	Returns a vector scaled */
struct vector Vector_Scalar(struct vector a, float scalar)
{
    return (struct vector) { a.x * scalar, a.y * scalar };
}

float Vector_Dot(struct vector a, struct vector b)
{
    return a.x * b.x + a.y * b.y;
}

/*
	Finds the cab distance between two vectors */
float Vector_CabDist(struct vector a, struct vector b)
{
    return fabs(a.x - b.x) + fabs(a.y - b.y);
}

/*
	Finds the euclidian distance between two vectors */
float Vector_Dist(struct vector a, struct vector b)
{
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

/*
	Returns the magnitude of a vector */
float Vector_Magnitude(struct vector a)
{
    return sqrtf(a.x * a.x + a.y * a.y);
}

/*
	Returns the angle of a vector */
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
