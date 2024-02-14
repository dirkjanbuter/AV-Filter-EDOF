#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

typedef struct VECTOR2D {
	float x;
	float y;
} VECTOR2D;

VECTOR2D *vector2d_create(float x, float y);
void vector2d_destroy(VECTOR2D *h);

float vector2d_length(VECTOR2D *v);
VECTOR2D *vector2d_normalize(VECTOR2D *v);
