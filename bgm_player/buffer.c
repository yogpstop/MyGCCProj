#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#ifndef _WIN32
const struct timespec ms = {0, 1000000};
#endif

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
	uint32_t fourcc;
	if (fread(&fourcc, 4, 1, f) != 1) fourcc = 0;
	fseek(f, 0, SEEK_SET);
	if (fourcc == 0x46464952) riff_read(f, ctx);
	else if (fourcc == 0x43614C66) flac_read(f, ctx);
	else if (fourcc == 0x01564C46) flv1_read(f, ctx);
}
THREAD_RAC buffer_thread(void *arg) {
	size_t cur = 0;
	buf_str *ctx = arg;
	MUTEX_LOCK(ctx->p.mutex + ctx->cur_id);
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
