#include "matrix3d.h"

MATRIX3D *matrix3d_zero() {
	int i;
	MATRIX3D *m = (MATRIX3D*)malloc(sizeof(MATRIX3D)*16);

	if (!m) return NULL;
	for (i = 0; i < 16; i++)
		m[i] = 0.0f;
	return m;
}

MATRIX3D *matrix3d_identity() {
	MATRIX3D *m = matrix3d_zero();
	if (!m) return NULL;
	m[0] = 1.0;
	m[5] = 1.0;
	m[10] = 1.0;
	m[15] = 1.0;
	return m;
}


MATRIX3D *matrix3d_transpose(MATRIX3D *m) {
	int i, j;
	MATRIX3D *o = NULL;

	if (!m) return NULL;
	o = matrix3d_zero();
	if (!o) return NULL;

	for (i = 0; i < 4; i++) for (j = 0; j < 4; j++)
		o[i * 4 + j] = m[j * 4 + i];
	return o;
}

//--Z - Axis:
//-- | cos  sin 0   0 |
//-- | -sin cos 0   0 |
//-- | 0    0   1   0 |
//-- | 0    0   0   1 |
MATRIX3D *matrix3d_rotationz(float angle) {
	MATRIX3D *o = NULL;
	float c, s;

	o = matrix3d_identity();
	if (!o) return NULL;
	c = (float)cos(angle);
	s = (float)sin(angle);
	o[0] = c;
	o[1] = s;
	o[4] = -s;
	o[5] = c;
	return o;
}

//-- Y - Axis:
//-- | cos  0 - sin  0 |
//-- | 0    1  0    0 |
//-- | sin  0  cos  0 |
//-- | 0    0  0    1 |
MATRIX3D *matrix3d_rotationy(float angle) {
	MATRIX3D *o = NULL;
	float c, s;

	o = matrix3d_identity();
	if (!o) return NULL;
	c = (float)cos(angle);
	s = (float)sin(angle);
	o[0] = c;
	o[2] = -s;
	o[8] = s;
	o[10] = c;
	return o;
}

//-- X - Axis:
//-- | 1  0    0    0 |
//-- | 0  cos  sin  0 |
//-- | 0 - sin  cos  0 |
//-- | 0  0    0    1 |
MATRIX3D *matrix3d_rotationx(float angle) {
	MATRIX3D *o = NULL;
	float c, s;

	o = matrix3d_identity();
	if (!o) return NULL;
	c = (float)cos(angle);
	s = (float)sin(angle);
	o[5] = c;
	o[6] = s;
	o[9] = -s;
	o[10] = c;
	return o;
}

MATRIX3D *matrix3d_translation(float x, float y, float z) {
	MATRIX3D *o = NULL;
	o = matrix3d_identity();
	if (!o) return NULL;
	o[12] = x;
	o[13] = y;
	o[14] = z;
	return o;
}

MATRIX3D *matrix3d_scale(float x, float y, float z) {
	MATRIX3D *o = NULL;
	o = matrix3d_identity();
	if (!o) return NULL;
	o[0] = x;
	o[5] = y;
	o[10] = z;
	return o;
}

MATRIX3D *matrix3d_multiply(MATRIX3D *m1, MATRIX3D *m2) {
	MATRIX3D *o = NULL;
	int x, y, n;

	if (!m1) return NULL;
	if (!m2) return NULL;

	o = matrix3d_zero();
	if (!o) return NULL;

	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			o[y * 4 + x] = 0.0f;
			for (n = 0; n < 4; n++)
				o[y * 4 + x] += m1[y * 4 + n] * m2[n * 4 + x];

		}
	}
	return o;
}


MATRIX3D *matrix3d_multiply3(MATRIX3D *m1, MATRIX3D *m2, MATRIX3D *m3) {
	MATRIX3D *t1 = NULL;
	MATRIX3D *t2 = NULL;
	MATRIX3D *t3 = NULL;
	MATRIX3D *o = NULL;

	t1 = matrix3d_identity();
	t2 = matrix3d_multiply(t1, m1);
	t3 = matrix3d_multiply(t2, m2);
	o = matrix3d_multiply(t3, m3);
	matrix3d_destroy(t3);
	matrix3d_destroy(t2);
	matrix3d_destroy(t1);
	return o;
}


void matrix3d_destroy(MATRIX3D *m) {
	if (!m) return;
	free(m);
	m = NULL;
}

VECTOR3D *matrix3d_vector(MATRIX3D *m, VECTOR3D *v) {
	int i;
	float res[4];
	VECTOR3D *vo = NULL;

	if (!m) return NULL;
	if (!v) return NULL;

	for (i = 0; i < 4; i++) {
		res[i] = v->x * m[0 * 4 + i] + v->y * m[1 * 4 + i] + v->z * m[2 * 4 + i] + m[3 * 4 + i];
	}
	vo = vector3d_create(
		res[0] / res[3],
		res[1] / res[3],
		res[2] / res[3]
	);
	return vo;
}

MATRIX3D *matrix3d_inverse(MATRIX3D *mt) {
	MATRIX3D *o;
	MATRIX3D *inv;
	float det = 0.0f;
	int i = 0;

	if (!mt) return NULL;
	o = matrix3d_identity();
	if (!o) return NULL;
	inv = matrix3d_identity();
	if (!inv) {
		matrix3d_destroy(o);
		return NULL;
	}
	inv[0] = mt[5] * mt[10] * mt[15] -
		mt[5] * mt[11] * mt[14] -
		mt[9] * mt[6] * mt[15] +
		mt[9] * mt[7] * mt[14] +
		mt[13] * mt[6] * mt[11] -
		mt[13] * mt[7] * mt[10];

	inv[4] = -mt[4] * mt[10] * mt[15] +
		mt[4] * mt[11] * mt[14] +
		mt[8] * mt[6] * mt[15] -
		mt[8] * mt[7] * mt[14] -
		mt[12] * mt[6] * mt[11] +
		mt[12] * mt[7] * mt[10];

	inv[8] = mt[4] * mt[9] * mt[15] -
		mt[4] * mt[11] * mt[13] -
		mt[8] * mt[5] * mt[15] +
		mt[8] * mt[7] * mt[13] +
		mt[12] * mt[5] * mt[11] -
		mt[12] * mt[7] * mt[9];

	inv[12] = -mt[4] * mt[9] * mt[14] +
		mt[4] * mt[10] * mt[13] +
		mt[8] * mt[5] * mt[14] -
		mt[8] * mt[6] * mt[13] -
		mt[12] * mt[5] * mt[10] +
		mt[12] * mt[6] * mt[9];

	inv[1] = -mt[1] * mt[10] * mt[15] +
		mt[1] * mt[11] * mt[14] +
		mt[9] * mt[2] * mt[15] -
		mt[9] * mt[3] * mt[14] -
		mt[13] * mt[2] * mt[11] +
		mt[13] * mt[3] * mt[10];

	inv[5] = mt[0] * mt[10] * mt[15] -
		mt[0] * mt[11] * mt[14] -
		mt[8] * mt[2] * mt[15] +
		mt[8] * mt[3] * mt[14] +
		mt[12] * mt[2] * mt[11] -
		mt[12] * mt[3] * mt[10];

	inv[9] = -mt[0] * mt[9] * mt[15] +
		mt[0] * mt[11] * mt[13] +
		mt[8] * mt[1] * mt[15] -
		mt[8] * mt[3] * mt[13] -
		mt[12] * mt[1] * mt[11] +
		mt[12] * mt[3] * mt[9];

	inv[13] = mt[0] * mt[9] * mt[14] -
		mt[0] * mt[10] * mt[13] -
		mt[8] * mt[1] * mt[14] +
		mt[8] * mt[2] * mt[13] +
		mt[12] * mt[1] * mt[10] -
		mt[12] * mt[2] * mt[9];

	inv[2] = mt[1] * mt[6] * mt[15] -
		mt[1] * mt[7] * mt[14] -
		mt[5] * mt[2] * mt[15] +
		mt[5] * mt[3] * mt[14] +
		mt[13] * mt[2] * mt[7] -
		mt[13] * mt[3] * mt[6];

	inv[6] = -mt[0] * mt[6] * mt[15] +
		mt[0] * mt[7] * mt[14] +
		mt[4] * mt[2] * mt[15] -
		mt[4] * mt[3] * mt[14] -
		mt[12] * mt[2] * mt[7] +
		mt[12] * mt[3] * mt[6];

	inv[10] = mt[0] * mt[5] * mt[15] -
		mt[0] * mt[7] * mt[13] -
		mt[4] * mt[1] * mt[15] +
		mt[4] * mt[3] * mt[13] +
		mt[12] * mt[1] * mt[7] -
		mt[12] * mt[3] * mt[5];

	inv[14] = -mt[0] * mt[5] * mt[14] +
		mt[0] * mt[6] * mt[13] +
		mt[4] * mt[1] * mt[14] -
		mt[4] * mt[2] * mt[13] -
		mt[12] * mt[1] * mt[6] +
		mt[12] * mt[2] * mt[5];

	inv[3] = -mt[1] * mt[6] * mt[11] +
		mt[1] * mt[7] * mt[10] +
		mt[5] * mt[2] * mt[11] -
		mt[5] * mt[3] * mt[10] -
		mt[9] * mt[2] * mt[7] +
		mt[9] * mt[3] * mt[6];

	inv[7] = mt[0] * mt[6] * mt[11] -
		mt[0] * mt[7] * mt[10] -
		mt[4] * mt[2] * mt[11] +
		mt[4] * mt[3] * mt[10] +
		mt[8] * mt[2] * mt[7] -
		mt[8] * mt[3] * mt[6];

	inv[11] = -mt[0] * mt[5] * mt[11] +
		mt[0] * mt[7] * mt[9] +
		mt[4] * mt[1] * mt[11] -
		mt[4] * mt[3] * mt[9] -
		mt[8] * mt[1] * mt[7] +
		mt[8] * mt[3] * mt[5];

	inv[15] = mt[0] * mt[5] * mt[10] -
		mt[0] * mt[6] * mt[9] -
		mt[4] * mt[1] * mt[10] +
		mt[4] * mt[2] * mt[9] +
		mt[8] * mt[1] * mt[6] -
		mt[8] * mt[2] * mt[5];

	det = mt[0] * inv[0] + mt[1] * inv[4] + mt[2] * inv[8] + mt[3] * inv[12];

	if (det == 0)
		return NULL;


	det = 1.f / det;

	for (i = 0; i < 16; i++) {
		o[i] = inv[i] * det;
	}

	return o;
}

MATRIX3D *matrix3d_lookat(VECTOR3D *eye, VECTOR3D *target, VECTOR3D *up) {
	VECTOR3D *vz = NULL;
	VECTOR3D *vx = NULL;
	VECTOR3D *vy = NULL;
	VECTOR3D *vt1 = NULL;
	MATRIX3D *mt1 = NULL;

	if (!eye) return NULL;
	if (!target) return NULL;
	if (!up) return NULL;

	vt1 = vector3d_create(eye->x - target->x, eye->y - target->y, eye->z - target->z);
	if (!vt1) return NULL;
	vz = vector3d_normalize(vt1);
	vector3d_destroy(vt1);
	if (!vz) return NULL;

	vt1 = vector3d_crossproduct(up, vz);
	if (!vt1) {
		vector3d_destroy(vz);
		return NULL;
	}
	vx = vector3d_normalize(vt1);
	vector3d_destroy(vt1);
	if (!vx) {
		vector3d_destroy(vz);
		return NULL;
	}

	vy = vector3d_crossproduct(vz, vx);
	if (!vy) {
		vector3d_destroy(vz);
		vector3d_destroy(vx);
		return NULL;
	}

	mt1 = matrix3d_zero();
	mt1[0] = vx->x;
	mt1[1] = vx->y;
	mt1[2] = vx->z;
	mt1[3] = 0;
	mt1[4] = vy->x;
	mt1[5] = vy->y;
	mt1[6] = vy->z;
	mt1[7] = 0;
	mt1[8] = vz->x;
	mt1[9] = vz->y;
	mt1[10] = vz->z;
	mt1[11] = 0;
	mt1[12] = eye->x;
	mt1[13] = eye->y;
	mt1[14] = eye->z;
	mt1[15] = 1;

	return matrix3d_inverse(mt1);
}

MATRIX3D *matrix3d_perspective(float fov, float aspect, float nearv, float farv) {
	MATRIX3D *o = matrix3d_zero();
	float D2R = (float)M_PI / 180.f;
	float yScale = 1.f / (float)tan(D2R*fov*0.5f);
	float xScale = yScale / aspect;
	float nearmfar = nearv - farv;
	o[0] = xScale;
	o[5] = yScale;
	o[10] = (farv + nearv) / nearmfar;
	o[11] = -1.0f;
	o[14] = 2.0f * farv*nearv / nearmfar;
	return o;
}
