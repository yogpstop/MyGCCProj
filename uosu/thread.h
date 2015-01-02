#include <stddef.h>
#include <pthread.h>
#include "osu.h"

typedef struct {
	unsigned int *done;
	void *ptr;
	size_t len;
	unsigned int *cur;
} pt_play;
void *play(pt_play*);
typedef struct {
	unsigned int *done;
	unsigned int *cur;
	pthread_mutex_t *mutex;
	unsigned int buf_len;
	hito *buf;
	difficulty *hard;
} pt_ppush;
void *ppush(pt_ppush*);
typedef struct {
	unsigned int *done;
	pthread_mutex_t *mutex;
	unsigned int buf_len;
	hito *buf;
	double cs;
} pt_glmain;
void *glmain(pt_glmain*);
