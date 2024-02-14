#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include "vector3d.h"

typedef float MATRIX3D;

MATRIX3D *matrix3d_zero();
MATRIX3D *matrix3d_identity();
MATRIX3D *matrix3d_transpose(MATRIX3D *m);
MATRIX3D *matrix3d_rotationz(float angle);
MATRIX3D *matrix3d_rotationy(float angle);
MATRIX3D *matrix3d_rotationx(float angle);
MATRIX3D *matrix3d_translation(float x, float y, float z);
MATRIX3D *matrix3d_scale(float x, float y, float z);
MATRIX3D *matrix3d_multiply(MATRIX3D *m1, MATRIX3D *m2);
MATRIX3D *matrix3d_multiply3(MATRIX3D *m1, MATRIX3D *m2, MATRIX3D *m3);
void matrix3d_destroy(MATRIX3D *m);
VECTOR3D *matrix3d_vector(MATRIX3D *m, VECTOR3D *v);
MATRIX3D *matrix3d_inverse(MATRIX3D *mt);
MATRIX3D *matrix3d_perspective(float fov, float aspect, float nearv, float farv);
MATRIX3D *matrix3d_lookat(VECTOR3D *eye, VECTOR3D *target, VECTOR3D *up);
