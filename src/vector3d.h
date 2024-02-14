#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

typedef struct VECTOR3D {
	float x;
	float y;
	float z;
} VECTOR3D;

VECTOR3D *vector3d_create(float x, float y, float z);
void vector3d_destroy(VECTOR3D *h);

float vector3d_length(VECTOR3D *v);
VECTOR3D *vector3d_normalize(VECTOR3D *v);
VECTOR3D *vector3d_crossproduct(VECTOR3D *a, VECTOR3D *b);
