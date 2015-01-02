#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "curve.h"

static inline int arc(point *ctr, double rad, unsigned int cdet,
		double bgn, double len, point *r, unsigned int rmax) {
	unsigned int max = ceil(cdet * len / (M_PI * 2));
	if (max < 2) max = 1;
	if (max + 1 > rmax) return -1;
	if (max == 1) {
		r->x = ctr->x + cos(bgn) * rad;
		r->y = ctr->y + sin(bgn) * rad;
		return 1;
	}
	int i;
	double t;
	for (i = 0; i < max; i++) {
		t = bgn + len * i / (max - 1);
		r[i].x = ctr->x + cos(t) * rad;
		r[i].y = ctr->y + sin(t) * rad;
	}
	return max;
}
#define CALC(px, py) \
		if ((p##px < t1 && t1 < t##px) ||  \
			(t##px < p##px && (p##px < t1 || t##px > t1))) { \
			t3 = t##px; t4 = p##px; \
		} else { t3 = p##px; t4 = t##px; } \
		if (t4 < t3) t4 += M_PI * 2; \
	px##loop: \
		tmp = arc(b + i, rad, cdet, t3, t4 - t3, \
				c##px, px##len - (c##px - r->px)); \
		if (tmp < 0) { \
			c##px = c##py + (c##px - r->px); \
			px##len *= 2; \
			r->px = realloc(r->px, px##len * sizeof(point)); \
			c##px = r->px + (c##px - c##py); \
			goto px##loop; \
		} \
		c##px += tmp;
void polygon(unsigned int bl, point *b, double cs,
		unsigned int cdet, slider_side *r) {
	double rad = 37.25 * (12 - cs) / 12;
	//double cth = M_PI * 2 / cdet;
	//r->alens = malloc((bl - 2) * sizeof(unsigned int));
	//r->blens = malloc((bl - 2) * sizeof(unsigned int));
	unsigned int alen, blen;
	alen = blen = bl;
	r->a = malloc(alen * sizeof(point));
	r->b = malloc(blen * sizeof(point));
	point *ca = r->a, *cb = r->b;
	unsigned int i;
	int tmp;
	double t1, t2, t3, t4, ta, tb, pa = DBL_MAX, pb = DBL_MAX;
	for (i = 1; i < bl - 1; i++) {
		t1 = atan2(b[i - 1].y - b[i].y, b[i - 1].x - b[i].x);
		t2 = atan2(b[i + 1].y - b[i].y, b[i + 1].x - b[i].x);
		t3 = t1 + (t2 - t1) / 2;
		     if (t3 >  M_PI) t3 -= M_PI * 2;
		else if (t3 < -M_PI) t3 += M_PI * 2;
		t4 = t3 + M_PI;
		     if (t4 >  M_PI) t4 -= M_PI * 2;
		if ((t1 < t3 && t3 < t2) || (t2 < t4 && t4 < t1)) {
			     ta = t3; tb = t4;
		} else { ta = t4; tb = t3; }
		CALC(a, b);
		CALC(b, a);
		pa = ta; pb = tb;
		//r->alens[i - 1] = ca - r->a; r->blens[i - 1] = cb - r->b;
	}
	r->alen = ca - r->a; r->blen = cb - r->b;
	r->a = realloc(r->a, (ca - r->a) * sizeof(point));
	r->b = realloc(r->b, (cb - r->b) * sizeof(point));
}
