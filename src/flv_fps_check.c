#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flv_core.h"
static unsigned char *diffs;
static unsigned int dlen = 0;
static unsigned int last = 0;
static int callback(uint8_t type, uint32_t ts, unsigned int len, void *dat) {
	if (type != 9) { return 0; }
	if (!len) { fprintf(stderr, "WARNING: invalid DataSize\n"); return 0; }
	if (ts - last > 255) { fprintf(stderr, "WARNING: too large time diff %d\n", ts - last); return 0; }
	diffs[dlen++] = ts - last;
	last = ts;
	return 0;
}
static int cmp_uc(const void *v1, const void *v2) {
	return *(unsigned char*)v1 - *(unsigned char*)v2;
}
static double avg_uc(const unsigned char * const i, const unsigned int m) {
	unsigned int n = m;
	unsigned long long r = 0;
	while (n--) r += i[n];
	return (double) r / m;
}
int main(int argc, char **argv) {
	diffs = malloc(1024 * 1024);
	flv_per_tag(argv[1], callback);
	qsort(diffs, dlen, sizeof(unsigned char), cmp_uc);
	printf("%12f %12f %12f %12f\n", (double) diffs[0], (double) diffs[dlen - 1], avg_uc(diffs, dlen), (double) diffs[dlen / 2]);
	free(diffs);
	return 0;
}
