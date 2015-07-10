#include <stdlib.h>
#include <string.h>
#include "main.h"
#define PERIOD_SIZE 48
#define CHANNELS 2
#define BITS 16
int exit_signal = 0, force_exit_signal = 0;
int main(int argc, char **argv) {
	int i;
	buf_str ctx;
	memset(&ctx, 0, sizeof(buf_str));
	ctx.p.period = PERIOD_SIZE;
	ctx.p.buf_max = 0xFFF;//TODO variable buf
	ctx.p.mutex = malloc(sizeof(MUTEX_T) * (ctx.p.buf_max + 1));
	for (i = 0; i <= ctx.p.buf_max; i++) {
		MUTEX_INIT(ctx.p.mutex + i);
	}
	ctx.p.update = malloc(sizeof(unsigned char) * (ctx.p.buf_max + 1));
	ctx.p.buf = malloc((ctx.p.buf_max + 1) * ctx.p.period * CHANNELS * BITS / 8);
	//FIXME memory allocation failed
	ctx.list = listing(argv + 1);
	if(!ctx.list) force_exit_signal = 1;
	else list_shuffle(ctx.list);
	THREAD_T play, buffer;
#ifndef _WIN32
	pthread_attr_t rt;
	struct sched_param p = {};
	p.sched_priority = sched_get_priority_max(SCHED_RR);
	pthread_attr_init(&rt);
	pthread_attr_setinheritsched(&rt, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&rt, SCHED_RR);
	pthread_attr_setschedparam(&rt, &p);
#endif
	CREATE_THREAD(buffer, buffer_thread, &ctx);
	//TODO wait buffering
	CREATE_THREAD_RT(play, play_thread, &ctx.p, &rt);
	console();
	JOIN_THREAD(buffer);
	JOIN_THREAD(play);
	list_full_remove(ctx.list);
	for (i = 0; i <= ctx.p.buf_max; i++) {
		MUTEX_DESTROY(ctx.p.mutex + i);
	}
	free(ctx.p.mutex);
	free(ctx.p.update);
	free(ctx.p.buf);
	return 0;
}
