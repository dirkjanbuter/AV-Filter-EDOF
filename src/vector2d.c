#include "vector2d.h"

VECTOR2D *vector2d_create(float x, float y) {
	VECTOR2D *v = (VECTOR2D*)malloc(sizeof(VECTOR2D));
	if (!v) return NULL;
	v->x = x;
	v->y = y;
	return v;
}

void vector2d_destroy(VECTOR2D *v) {
	if (!v) return;
	free(v);
	v = NULL;
}

float vector2d_length(VECTOR2D *v) {
	if (!v) return 0;
	return (float)sqrt(v->x * v->x + v->y * v->y);
}

VECTOR2D *vector2d_normalize(VECTOR2D *v) {
	float length = 0.0f;
	VECTOR2D *o = NULL;

	if (!v) return NULL;
	length = vector2d_length(v);
	o = vector2d_create(v->x, v->y);
	if (!o) return NULL;

	if (length != 0) {
		o->x = v->x / length;
		o->y = v->y / length;
	}
	return o;
}

