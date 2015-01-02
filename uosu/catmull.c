#include <stdlib.h>
#include "curve.h"

static inline double getv(double v1, double v2, double v3, double v4, double t) {
	return 0.5 * (t * t * t * (-     v1 + 3 * v2 - 3 * v3 + v4)
				+     t * t * (+ 2 * v1 - 5 * v2 + 4 * v3 - v4)
				+         t * (-     v1          +     v3)
				+                       + 2 * v2);
}
point *catmull(unsigned int bl, point *b, unsigned int *rl) {
	unsigned int i, k, rp = 0;
	double pos;
	point *v1, *v2, *v3, *v4, tmp;
	*rl = --bl * SLIDER_DETAIL_LEVEL;
	point *r = malloc(*rl * sizeof(point));
	for (i = 0; i < bl; i++) {
		v1 = b + i - (i > 0 ? 1 : 0);
		v2 = b + i;
		v3 = b + i + 1;
		if (i + 1 < bl)
			v4 = b + i + 2;
		else {
			v4 = &tmp;
			tmp.x = v3->x + v3->x - v2->x;
			tmp.y = v3->y + v3->y - v2->y;
		}
		for (k = 0; k < SLIDER_DETAIL_LEVEL; k++) {
			pos = (double) k / (SLIDER_DETAIL_LEVEL - 1);
			r[rp  ].x = getv(v1->x, v2->x, v3->x, v4->x, pos);
			r[rp++].y = getv(v1->y, v2->y, v3->y, v4->y, pos);
		}
	}
	return r;
}
