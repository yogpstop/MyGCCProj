#include <stdlib.h>
#include <string.h>
#include "curve.h"

static inline unsigned int _bezier(unsigned int bl, point *b, point *r) {
	point *tmp = malloc(sizeof(point) * bl);
	int i, j, k;
	double pos;
	for (k = 0; k < SLIDER_DETAIL_LEVEL * (bl - 1); k++) {
		memcpy(tmp, b, sizeof(point) * bl);
		pos = (double) k / (SLIDER_DETAIL_LEVEL * (bl - 1));
		for (i = bl - 1; i > 0; i--) {
			for (j = 0; j < i; j++) {
				tmp[j].x = tmp[j].x + (tmp[j + 1].x - tmp[j].x) * pos;
				tmp[j].y = tmp[j].y + (tmp[j + 1].y - tmp[j].y) * pos;
			}
		}
		r[k].x = tmp->x;
		r[k].y = tmp->y;
	}
	free(tmp);
	return k;
}
point *bezier(unsigned int bl, point *b, unsigned int *rl) {
	*rl = (bl - 1) * SLIDER_DETAIL_LEVEL;
	point *r = malloc(*rl * sizeof(point)), *lr = r;
	unsigned int i, l = 0;
	for (i = 0; i < bl - 1; i++) {
		if (b[i].x == b[i + 1].x && b[i].y == b[i + 1].y) {
			lr += _bezier(i - l + 1, b + l, lr); l = ++i; } }
	lr += _bezier(bl - l, b + l, lr);
	*rl = lr - r;
	return realloc(r, *rl * sizeof(point));
}
