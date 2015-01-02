#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include "curve.h"

void uniform(unsigned int bl, point *b, unsigned int rl, point *r) {
	double *lens = malloc(bl * sizeof(double));
	double *tots = malloc(bl * sizeof(double));
	double curlen;
	unsigned int i, bp = 0;
	tots[0] = 0;
	for (i = 1; i < bl; i++) {
		lens[i] = sqrt((b[i].x - b[i - 1].x) * (b[i].x - b[i - 1].x)
					 + (b[i].y - b[i - 1].y) * (b[i].y - b[i - 1].y));
		tots[i] = tots[i - 1] + lens[i];
	}
	for (i = 0; i < rl; i++) {
		curlen = tots[bl - 1] * i / (rl - 1);
		while (bp < bl - 2 && curlen > tots[bp + 1]) bp++;
		r[i].x = b[bp].x +
				(b[bp + 1].x - b[bp].x) * (curlen - tots[bp]) / lens[bp + 1];
		r[i].y = b[bp].y +
				(b[bp + 1].y - b[bp].y) * (curlen - tots[bp]) / lens[bp + 1];
	}
	free(lens);
	free(tots);
}
