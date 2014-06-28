#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
void *riff_read(FILE *f, int *size, size_t from, size_t to) {
	uint32_t rbuf[11] = {};
	void *data = NULL, *ptr;
	int rem = 0, read;
	if (fread(rbuf, 44, 1, f) == 1) {
		rem = rbuf[10];
		if ((rbuf[1] - 36) == rem) {
			fseek(f, from, SEEK_CUR);
			if (rem > to) rem = to;
			rem -= from;
			rbuf[10] = rem;
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
