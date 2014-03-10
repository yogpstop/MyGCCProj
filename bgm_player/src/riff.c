#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
void *riff_read(FILE *f, int *size) {
	uint32_t rbuf[11];
	if (fread(rbuf, 44, 1, f) != 1) return NULL;
	int read, rem = rbuf[10];
	if ((rbuf[1] - 36) != rem) return NULL;
	void *data = malloc(rem), *ptr = data;
	while((read = fread(ptr, 1, rem, f)) >= 0) {
		ptr += read;
		rem -= read;
		if (rem <= 0) break;
	}
	fclose(f);
	*size = rbuf[10] - rem;
	return data;
}
