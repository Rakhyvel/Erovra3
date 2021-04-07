/*
vector.h
*/

#pragma once
typedef struct vector {
	float x;
	float y;
} Vector;

float Vector_Dist(struct vector* a, struct vector* b);