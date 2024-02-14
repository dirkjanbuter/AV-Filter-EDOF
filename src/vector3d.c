#include "vector3d.h"

VECTOR3D *vector3d_create(float x, float y, float z) {
	VECTOR3D *v = (VECTOR3D*)malloc(sizeof(VECTOR3D));
	if (!v) return NULL;
	v->x = x;
	v->y = y;
	v->z = z;
	return v;
}

void vector3d_destroy(VECTOR3D *v) {
	if (!v) return;
	free(v);
	v = NULL;
}

float vector3d_length(VECTOR3D *v) {
	if (!v) return 0;
	return (float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

VECTOR3D *vector3d_normalize(VECTOR3D *v) {
	float length = 0.0f;
	VECTOR3D *o = NULL;

	if (!v) return NULL;
	length = vector3d_length(v);
	o = vector3d_create(v->x, v->y, v->z);
	if (!o) return NULL;

	if (length != 0) {
		o->x = v->x / length;
		o->y = v->y / length;
		o->z = v->z / length;
	}
	return o;
}

VECTOR3D *vector3d_crossproduct(VECTOR3D *a, VECTOR3D *b) {
	VECTOR3D *o = NULL;

	if (!a) return NULL;
	if (!b) return NULL;

	o = vector3d_create(0.0f, 0.0f, 0.0f);
	if (!o) return NULL;

	o->x = a->y*b->z - b->y*a->z;
	o->y = b->x*a->z - a->x*b->z;
	o->z = a->x*b->y - a->y*b->x;
	return o;
}
