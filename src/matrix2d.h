#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include "vector2d.h"

typedef float MATRIX2D;

MATRIX2D *matrix2d_zero();
MATRIX2D *matrix2d_identity();
MATRIX2D *matrix2d_transpose(MATRIX2D *m);
MATRIX2D *matrix2d_rotationz(float angle);
MATRIX2D *matrix2d_rotationy(float angle);
MATRIX2D *matrix2d_rotationx(float angle);
MATRIX2D *matrix2d_translation(float x, float y);
MATRIX2D *matrix2d_scale(float x, float y);
MATRIX2D *matrix2d_multiply(MATRIX2D *m1, MATRIX2D *m2);
MATRIX2D *matrix2d_multiply3(MATRIX2D *m1, MATRIX2D *m2, MATRIX2D *m3);
void matrix2d_destroy(MATRIX2D *m);
VECTOR2D *matrix2d_vector(MATRIX2D *m, VECTOR2D *v);
MATRIX2D *matrix2d_inverse(MATRIX2D *mt);
