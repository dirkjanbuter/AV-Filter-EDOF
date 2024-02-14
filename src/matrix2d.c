#include "matrix2d.h"

MATRIX2D *matrix2d_zero() {
	int i;
	MATRIX2D *m = (MATRIX2D*)malloc(sizeof(MATRIX2D)*9);

	if (!m) return NULL;
	for (i = 0; i < 9; i++)
		m[i] = 0.0f;
	return m;
}

void matrix2d_destroy(MATRIX2D *m) {
	if (!m) return;
	free(m);
	m = NULL;
}

MATRIX2D *matrix2d_identity() {
	MATRIX2D *m = matrix2d_zero();
	if (!m) return NULL;
	m[0] = 1.0;
	m[4] = 1.0;
	m[8] = 1.0;
	return m;
}

MATRIX2D *matrix2d_transpose(MATRIX2D *m) {
	int i, j;
	MATRIX2D *o = NULL;

	if (!m) return NULL;
	o = matrix2d_zero();
	if (!o) return NULL;

	for (i = 0; i < 3; i++) for (j = 0; j < 3; j++)
		o[i * 3 + j] = m[j * 3 + i];
	return o;
}

//--Z - Axis:
//-- | cos -sin  0 |
//-- | sin  cos  0 |
//-- | 0     0   1 |
MATRIX2D *matrix2d_rotationz(float angle) {
	MATRIX2D *o = NULL;
	float c, s;

	o = matrix2d_identity();
	if (!o) return NULL;
	c = (float)cos(angle);
	s = (float)sin(angle);
	o[0] = c;
	o[1] = -s;
	o[3] = s;
	o[4] = c;
	return o;
}

//-- Y - Axis:
//-- | cos  0  sin |
//-- | 0    1  0   |
//-- |-sin  0  cos |
MATRIX2D *matrix2d_rotationy(float angle) {
	MATRIX2D *o = NULL;
	float c, s;

	o = matrix2d_identity();
	if (!o) return NULL;
	c = (float)cos(angle);
	s = (float)sin(angle);
	o[0] = c;
	o[2] = s;
	o[6] = -s;
	o[8] = c;
	return o;
}

//-- X - Axis:
//-- | 1  0    0    |
//-- | 0  cos -sin  |
//-- | 0  sin  cos  |
MATRIX2D *matrix2d_rotationx(float angle) {
	MATRIX2D *o = NULL;
	float c, s;

	o = matrix2d_identity();
	if (!o) return NULL;
	c = (float)cos(angle);
	s = (float)sin(angle);
	o[4] = c;
	o[5] = -s;
	o[7] = s;
	o[8] = c;
	return o;
}

MATRIX2D *matrix2d_translation(float x, float y) {
	MATRIX2D *o = NULL;
	o = matrix2d_identity();
	if (!o) return NULL;
	o[6] = x;
	o[7] = y;
	return o;
}

MATRIX2D *matrix2d_scale(float x, float y) {
	MATRIX2D *o = NULL;
	o = matrix2d_identity();
	if (!o) return NULL;
	o[0] = x;
	o[4] = y;
	return o;
}

MATRIX2D *matrix2d_multiply(MATRIX2D *m1, MATRIX2D *m2) {
	MATRIX2D *o = NULL;
	int x, y, n;

	if (!m1) return NULL;
	if (!m2) return NULL;

	o = matrix2d_zero();
	if (!o) return NULL;

	for (y = 0; y < 3; y++) {
		for (x = 0; x < 3; x++) {
			o[y * 3 + x] = 0.0f;
			for (n = 0; n < 3; n++)
				o[y * 3 + x] += m1[y * 3 + n] * m2[n * 3 + x];

		}
	}
	return o;
}

MATRIX2D *matrix2d_multiply3(MATRIX2D *m1, MATRIX2D *m2, MATRIX2D *m3) {
	MATRIX2D *t1 = NULL;
	MATRIX2D *t2 = NULL;
	MATRIX2D *t3 = NULL;
	MATRIX2D *o = NULL;

	t1 = matrix2d_identity();
	t2 = matrix2d_multiply(t1, m1);
	t3 = matrix2d_multiply(t2, m2);
	o = matrix2d_multiply(t3, m3);
	matrix2d_destroy(t3);
	matrix2d_destroy(t2);
	matrix2d_destroy(t1);
	return o;
}

VECTOR2D *matrix2d_vector(MATRIX2D *m, VECTOR2D *v) {
	int i;
	float res[3];
	VECTOR2D *vo = NULL;

	if (!m) return NULL;
	if (!v) return NULL;

	for (i = 0; i < 3; i++) {
		res[i] = v->x * m[0 + i] + v->y * m[3 + i] + m[6 + i];
	}
	vo = vector2d_create(
		res[0] / res[2],
		res[1] / res[2]
	);
	return vo;
}

MATRIX2D *matrix2d_inverse(MATRIX2D *m) {
    double det = m[0] * (m[4] * m[8] - m[7] * m[5]) -
                 m[1] * (m[3] * m[8] - m[5] * m[6]) +
                 m[2] * (m[3] * m[7] - m[4] * m[6]);

    double invdet = 1 / det;
    MATRIX2D *minv = matrix2d_zero();

    minv[0] = (m[4] * m[8] - m[7] * m[5]) * invdet;
    minv[1] = (m[2] * m[7] - m[1] * m[8]) * invdet;
    minv[2] = (m[1] * m[5] - m[2] * m[4]) * invdet;
    minv[3] = (m[5] * m[3] - m[3] * m[8]) * invdet;
    minv[4] = (m[0] * m[5] - m[2] * m[6]) * invdet;
    minv[5] = (m[3] * m[2] - m[0] * m[5]) * invdet;
    minv[6] = (m[3] * m[7] - m[3] * m[4]) * invdet;
    minv[7] = (m[6] * m[1] - m[0] * m[7]) * invdet;
    minv[8] = (m[0] * m[4] - m[3] * m[1]) * invdet;
    return minv;
}
