#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "osu.h"
#include "thread.h"
#include "fsutil.h"

void readosu(const char*, hito**, unsigned int*, colours*,
		timing**, unsigned int*, difficulty*, char**);
void *mp3(char *, size_t *);
#define INIT_COLOR(c) c.r=-1; c.g=-1; c.b=-1;
#define SET_COLOR(c, pr, pg, pb) \
	c.r = ((double) pr) / 255; \
	c.g = ((double) pg) / 255; \
	c.b = ((double) pb) / 255;
#define INIT_COLOURS(u) \
	SET_COLOR(u.combo[0], 255, 150, 0) \
	SET_COLOR(u.combo[1], 5, 240, 5) \
	SET_COLOR(u.combo[2], 5, 5, 240) \
	SET_COLOR(u.combo[3], 240, 5, 5) \
	INIT_COLOR(u.combo[4]) \
	INIT_COLOR(u.combo[5]) \
	INIT_COLOR(u.combo[6]) \
	INIT_COLOR(u.combo[7]) \
	SET_COLOR(u.border, 255, 255, 255)
int main(int argc, char **argv) {
	int pwdfd = open(".", O_RDONLY);
	int i;
	for (i = 1; i < argc; i++) {
		char *osufn = abspath(argv[i]);
		char *ddir = dirname(osufn);
		if (ddir) { chdir(ddir); free(ddir); }
		unsigned int holen, tlen;
		hito *ho = NULL;
		timing *t = NULL;
		difficulty hard;
		colours col;
		INIT_COLOURS(col);
		char* mp3fn = NULL;
		readosu(osufn, &ho, &holen, &col, &t, &tlen, &hard, &mp3fn);
		free(osufn);
		pt_play s_play;
		s_play.ptr = mp3(mp3fn, &s_play.len);
		free(mp3fn);
		fchdir(pwdfd);
		unsigned int cur = 0;
		pt_ppush s_ppush;
		pt_glmain s_glmain;
		s_ppush.cur = s_play.cur = &cur;
		s_glmain.buf = s_ppush.buf = ho;
		s_glmain.buf_len = s_ppush.buf_len = holen;
		s_glmain.cs = hard.cs;
		s_ppush.hard = &hard;
		pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
		s_glmain.mutex = s_ppush.mutex = &mutex;
		unsigned int done = 0;
		s_glmain.done = s_ppush.done = s_play.done = &done;
		pthread_t t_play, t_ppush, t_glmain;
		pthread_attr_t rt;
		struct sched_param p;
		p.sched_priority = sched_get_priority_max(SCHED_RR);
		pthread_attr_init(&rt);
		pthread_attr_setinheritsched(&rt, PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&rt, SCHED_RR);
		pthread_attr_setschedparam(&rt, &p);
		pthread_create(&t_glmain, NULL, (void*(*)(void*))&glmain, &s_glmain);
		pthread_create(&t_ppush, NULL, (void*(*)(void*))&ppush, &s_ppush);
		pthread_create(&t_play, &rt, (void*(*)(void*))&play, &s_play);
		pthread_join(t_play, NULL);
		pthread_join(t_ppush, NULL);
		pthread_join(t_glmain, NULL);
	}
	close(pwdfd);
	return 0;
}
