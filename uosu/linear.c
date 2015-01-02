#include <stdlib.h>
#include "curve.h"

point *linear(unsigned int bl, point *b, unsigned int *rl) {
	*rl = --bl * SLIDER_DETAIL_LEVEL;
	point *r = malloc(*rl * sizeof(point));
	int i, j;
	double pos, xlen, ylen;
	for (i = 0; i < bl; i++) {
		xlen = b[i + 1].x - b[i].x;
		ylen = b[i + 1].y - b[i].y;
		for (j = 0; j < SLIDER_DETAIL_LEVEL; j++) {
			pos = (double) j / (SLIDER_DETAIL_LEVEL - 1);
			r[i * SLIDER_DETAIL_LEVEL + j].x = b[i].x + pos * xlen;
			r[i * SLIDER_DETAIL_LEVEL + j].y = b[i].y + pos * ylen;
		}
	}
	return r;
}
