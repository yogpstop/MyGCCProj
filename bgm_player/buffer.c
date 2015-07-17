#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#define CHANNELS 2
#define BITS 16
#ifndef _WIN32
const struct timespec ms = {0, 1000000};
#endif

void bytes2buf(void *dest, size_t done, buf_str *ctx) {
	size_t len;
	//FIXME if (done & 3)
	if (ctx->to <= done) { done = ctx->to; ctx->to = 0; }
	else { ctx->to -= done; }
	if (ctx->from >= done) { ctx->from -= done; return; }
	else if (ctx->from > 0) { done -= ctx->from; dest += ctx->from; ctx->from = 0; }
	while (done) {
		len = (ctx->p.period - ctx->cur_period) * CHANNELS * BITS / 8;
		if (len > done) len = done;
		memcpy(ctx->p.buf + (ctx->cur_id * ctx->p.period + ctx->cur_period) * CHANNELS * BITS / 8, dest, len);
		done -= len;
		dest += len;
		ctx->cur_period += len / (CHANNELS * BITS / 8);
		if (ctx->cur_period >= ctx->p.period) {
			ctx->cur_id++;
			ctx->cur_id = ctx->cur_id & ctx->p.buf_max;
			MUTEX_LOCK(ctx->p.mutex + ctx->cur_id);
			MUTEX_UNLOCK(ctx->p.mutex + ((ctx->cur_id - 1) & ctx->p.buf_max));
			ctx->cur_period = 0;
		}
	}
}

static inline void buffering_do(buf_str *ctx, size_t idx) {
	ctx->p.name = malloc((strlen(ctx->list[idx].n) + 1) * sizeof(char));
	if (!ctx->p.name) return;
	ctx->p.update[ctx->cur_id] = 0xFF;
	strcpy(ctx->p.name, strrchr(ctx->list[idx].n, DIR_SEP) + 1);
	char *c = strrchr(ctx->p.name, '.');
	if (c) *c = 0;
	ctx->from = ctx->list[idx].f;
	ctx->to = ctx->list[idx].t;
	FILE *f = fopen(ctx->list[idx].n, "rb");
	uint32_t fourcc[2];
	if (fread(fourcc, 4, 2, f) < 2) { fourcc[0] = 0; fourcc[1] = 0; }
	fseek(f, 0, SEEK_SET);
	if (fourcc[0] == 0x46464952) riff_read(f, ctx);
	else if (fourcc[0] == 0x43614C66) flac_read(f, ctx);
	else if (fourcc[0] == 0x01564C46) flv1_read(f, ctx);
	else if (fourcc[1] == 0x70797466) mp4_read(f, ctx);
}
THREAD_RAC buffer_thread(void *arg) {
	size_t cur = 0;
	buf_str *ctx = arg;
	volatile unsigned char *sync = ctx->p.update + ctx->cur_id;
	MUTEX_LOCK(ctx->p.mutex + ctx->cur_id);
	*sync = 0xFF;
	while (*sync);
	while(1) {
		if (force_exit_signal) break;
		buffering_do(ctx, cur++);
		if (!ctx->list[cur].n) {
			list_shuffle(ctx->list);
			cur = 0;
		}
	}
	MUTEX_UNLOCK(ctx->p.mutex + ctx->cur_id);
	return 0;
}
