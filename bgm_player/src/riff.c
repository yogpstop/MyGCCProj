#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
void *riff_read(FILE *f, int *size) {
	uint32_t rbuf[11] = {};
	void *data = NULL, *ptr;
	int rem = 0, read;
	if (fread(rbuf, 44, 1, f) == 1) {
		rem = rbuf[10];
		if ((rbuf[1] - 36) == rem) {
			ptr = data = malloc(rem);
			while((read = fread(ptr, 1, rem, f)) >= 0) {
				ptr += read;
				rem -= read;
				if (rem <= 0) break;
			}
		}
	}
	fclose(f);
	*size = rbuf[10] - rem;
	return data;
}
