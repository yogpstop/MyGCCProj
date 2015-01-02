#include <math.h>
#include <stdlib.h>
#include "curve.h"

point *pass(unsigned int bl, point *b, unsigned int *rl) {
	double La = 1 / ((b[0].y - b[1].y) / (b[0].x - b[1].x));
	double Ma = 1 / ((b[1].y - b[2].y) / (b[1].x - b[2].x));
	double Lb = b[0].y + (b[1].y - b[0].y) / 2 - La * (b[0].x + (b[1].x - b[0].x) / 2);
	double Mb = b[1].y + (b[2].y - b[1].y) / 2 - Ma * (b[1].x + (b[2].x - b[1].x) / 2);
	point ctr;
	ctr.x = (Lb - Mb) / (Ma - La);
	ctr.y = La * ctr.x + Lb;
	/*double D = 2 * (b[0].x * (b[1].y - b[2].y) +
					b[1].x * (b[2].y - b[0].y) +
					b[2].x * (b[0].y - b[1].y));
	point ctr;
	ctr.x = ((b[0].x * b[0].x + b[0].y * b[0].y) * (b[1].y - b[2].y) +
			 (b[1].x * b[1].x + b[1].y * b[1].y) * (b[2].y - b[0].y) +
			 (b[2].x * b[2].x + b[2].y * b[2].y) * (b[0].y - b[1].y)) / D;
	ctr.y = ((b[0].x * b[0].x + b[0].y * b[0].y) * (b[2].x - b[1].x) +
			 (b[1].x * b[1].x + b[1].y * b[1].y) * (b[0].x - b[2].x) +
			 (b[2].x * b[2].x + b[2].y * b[2].y) * (b[1].x - b[0].x)) / D;*/
	double rad = sqrt((ctr.x - b[0].x) * (ctr.x - b[0].x)
					+ (ctr.y - b[0].y) * (ctr.y - b[0].y));
	double t0 = atan2(b[0].y - ctr.y, b[0].x - ctr.x);
	double t1 = atan2(b[1].y - ctr.y, b[1].x - ctr.x);
	double t2 = atan2(b[2].y - ctr.y, b[2].x - ctr.x);
	double bgn, len;
	if ((t0 < t1 && t1 < t2) || 
		(t2 < t0 && (t0 < t1 || t1 < t2))) {
		bgn = t0; len = t2;
	} else {
		bgn = t2; len = t0;
	}
	if (bgn > len) len += M_PI * 2;
	len = len - bgn;
	int k;
	double cur;
	*rl = (bl - 1) * SLIDER_DETAIL_LEVEL;
	point *r = malloc(*rl * sizeof(point));
	for (k = 0; k < *rl; k++) {
		cur = bgn + len * k / (*rl - 1);
		r[k].x = ctr.x + cos(cur) * rad;
		r[k].y = ctr.y + sin(cur) * rad;
	}
	return r;
}
