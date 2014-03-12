#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
extern char **list;
extern size_t list_size;
const struct timespec ms = {0, 1000000};
typedef struct {
	char *n;
	void *d;
	int l;
} pcm;
extern pcm buf[2];
extern int force_exit_signal;
void list_shuffle();
void *flac_read(FILE *, int *);
void *riff_read(FILE *, int *);
static inline void buffering_do(char *n) {
	int cur_id = 0;
	pcm *str = buf + cur_id;
	while(str->d != NULL || str->l != 0 || str->n != NULL) {
		if(force_exit_signal) return;
		nanosleep(&ms, NULL);
		cur_id = cur_id ? 0 : 1;
		str = buf + cur_id;
	}
	str->n = malloc((strlen(n)+1)*sizeof(char));
	strcpy(str->n, strrchr(n, '/') + 1);
	*strrchr(str->n, '.') = 0;
	FILE *f = fopen(n, "rb");
	uint32_t fourcc;
	if (fread(&fourcc, 4, 1, f) != 1) fourcc = 0;
	fseek(f, 0, SEEK_SET);
	if(fourcc == 0x46464952) str->d = riff_read(f, &(str->l));
	else if(fourcc == 0x43614C66) str->d = flac_read(f, &(str->l));
	if (str->d == NULL || str->l == 0 || str->n == NULL) {
		if(str->d != NULL) free(str->d);
		str->d = NULL;
		if(str->n != NULL) free(str->n);
		str->n = NULL;
		str->l = 0;
	}
	fclose(f);
}
void clear_buffer() {
	if(buf[0].d != NULL) free(buf[0].d);
	buf[0].d = NULL;
	if(buf[0].n != NULL) free(buf[0].n);
	buf[0].n = NULL;
	buf[0].l = 0;
	if(buf[1].d != NULL) free(buf[1].d);
	buf[1].d = NULL;
	if(buf[1].n != NULL) free(buf[1].n);
	buf[1].n = NULL;
	buf[1].l = 0;
}
void *buffer_thread(void *_) {
	int cur = 0;
	while(1) {
		if (force_exit_signal) break;
		buffering_do(list[cur++]);
		if (cur>=list_size) {
			list_shuffle();
			cur = 0;
		}
	}
	return NULL;
}
