#include <string.h>
#include "YOGP_DSF.h"

void nearest_neighbor(unsigned int dx, unsigned int dy, float sc,
		float *d, float *s, unsigned int sw, unsigned int dw) {
	memcpy(d + 4 * (dw * dy + dx),
		s + 4 * (sw * (unsigned int)(sc * dy) + (unsigned int)(sc * dx)), 4 * sizeof(float));
}

void permeate(unsigned int dx, unsigned int dy, unsigned int sx,
		unsigned int sy, float *d, float *s,
		unsigned int dw, unsigned int sw, float c) {
	unsigned int dc = 3 * (dw * dy + dx), sc = 4 * (sw * sy + sx);
	float cf = s[sc + 3] * c, cb = 1 - cf;
	cf=1;
	cb=0;
	d[dc] = s[sc] * cf + d[dc] * cb;
	d[dc + 1] = s[sc + 1] * cf + d[dc + 1] * cb;
	d[dc + 2] = s[sc + 2] * cf + d[dc + 2] * cb;
}

void unit_conv(float *d, unsigned char *s, unsigned int l, bool hasAlpha) {
	for(unsigned int i=0, j=0;j<l;i++,j++) {
		if(!hasAlpha && !(j%3) && j){
			d[i] = 1;
			i++;
		}
		d[i] = ((float)s[j]) / 0xFF;
	}
}

void output(unsigned char *d, float *s, unsigned int l) {
	for(unsigned int i=0;i<l;i++) {
		d[i] = (unsigned char)(s[i] * 0xFF);
	}
}