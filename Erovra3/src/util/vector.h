/*
vector.h
*/

#pragma once
typedef struct vector {
	float x;
	float y;
} Vector;

void Vector_Copy(struct vector* a, struct vector* b);
struct vector Vector_Add(struct vector* a, struct vector* b);
struct vector Vector_Sub(struct vector* a, struct vector* b);
struct vector Vector_Scalar(struct vector* a, float scalar);
float Vector_Dist(struct vector* a, struct vector* b);
float Vector_Angle(struct vector* a);