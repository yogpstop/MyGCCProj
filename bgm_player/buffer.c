#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list.h"

const struct timespec ms = {0, 1000000};
typedef struct {
	char *n;
	void *d;
	int l;
} pcm;
extern pcm buf[2];
extern int force_exit_signal;
void list_shuffle();
void *flac_read(FILE *, int *, size_t, size_t);
void *riff_read(FILE *, int *, size_t, size_t);
static inline void buffering_do(data_format *n) {
	int cur_id = 0;
	pcm *str = buf + cur_id;
	while(str->d || str->l || str->n) {
		if(force_exit_signal) return;
		nanosleep(&ms, NULL);
		cur_id = cur_id ? 0 : 1;
		str = buf + cur_id;
	}
	str->n = malloc((strlen(n->n) + 1) * sizeof(char));
	if (!str->n) return;
	strcpy(str->n, strrchr(n->n, '/') + 1);
	*strrchr(str->n, '.') = 0;
	FILE *f = fopen(n->n, "rb");
	uint32_t fourcc;
	if (fread(&fourcc, 4, 1, f) != 1) fourcc = 0;
	fseek(f, 0, SEEK_SET);
	if(fourcc == 0x46464952) str->d = riff_read(f, &(str->l), n->f, n->t);
	else if(fourcc == 0x43614C66) str->d = flac_read(f, &(str->l), n->f, n->t);
	if (!str->d || !str->l || !str->n) {
		if(str->d) free(str->d);
		str->d = NULL;
		if(str->n) free(str->n);
		str->n = NULL;
		str->l = 0;
	}
}
void clear_buffer() {
	if(buf[0].d) free(buf[0].d);
	buf[0].d = NULL;
	if(buf[0].n) free(buf[0].n);
	buf[0].n = NULL;
	buf[0].l = 0;
	if(buf[1].d) free(buf[1].d);
	buf[1].d = NULL;
	if(buf[1].n) free(buf[1].n);
	buf[1].n = NULL;
	buf[1].l = 0;
}
void *buffer_thread(void *_) {
	int cur = 0;
	while(1) {
		if (force_exit_signal) break;
		buffering_do(list + cur++);
		if (cur>=list_size) {
			list_shuffle();
			cur = 0;
		}
	}
	return NULL;
}
