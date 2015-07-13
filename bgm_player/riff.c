#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "main.h"
#define CHANNELS 2
#define BITS 16
// multiple data tag is unsupported
void riff_read(FILE *f, buf_str *ctx) {
	uint32_t rbuf[2];
	void *ptr;
	int rem = 0, read;
	if (fseek(f, 12, SEEK_CUR)) goto Lexit;
	do {
		if (fread(rbuf, 8, 1, f) != 1) goto Lexit;
	} while (rbuf[0] != 0x61746164 && !fseek(f, rbuf[1], SEEK_CUR));
	if (fseek(f, ctx->from, SEEK_CUR)) goto Lexit;
	if (rbuf[1] > ctx->to) rbuf[1] = ctx->to;
	if (rbuf[1] < ctx->from) goto Lexit;
	rbuf[1] -= ctx->from;
	while (rbuf[1]) {
		rem = (ctx->p.period - ctx->cur_period) * CHANNELS * BITS / 8;
		ptr = ctx->p.buf + (ctx->cur_id * ctx->p.period + ctx->cur_period) * CHANNELS * BITS / 8;
		while ((read = fread(ptr, 1, rem > rbuf[1] ? rbuf[1] : rem, f)) > 0) {
			ptr += read; rem -= read; rbuf[1] -= read;
		}
		if (rem <= 0) {
			ctx->cur_id++;
			ctx->cur_id = ctx->cur_id & ctx->p.buf_max;
			MUTEX_LOCK(ctx->p.mutex + ctx->cur_id);
			MUTEX_UNLOCK(ctx->p.mutex + ((ctx->cur_id - 1) & ctx->p.buf_max));
			ctx->cur_period = 0;
		} else ctx->cur_period = ctx->p.period - rem / (CHANNELS * BITS / 8);
		if (feof(f)) break;
	}
Lexit:
	fclose(f);
}
