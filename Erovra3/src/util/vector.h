/*
vector.h
*/

#pragma once
typedef struct vector {
	float x;
	float y;
} Vector;

struct vector Vector_Normalize(struct vector a);
struct vector Vector_Add(struct vector a, struct vector b);
struct vector Vector_Sub(struct vector a, struct vector b);
struct vector Vector_Scalar(struct vector a, float scalar);
float Vector_Dot(struct vector a, struct vector b);
float Vector_CabDist(struct vector a, struct vector b);
float Vector_Dist(struct vector a, struct vector b);
float Vector_Magnitude(struct vector a);
float Vector_Angle(struct vector a);