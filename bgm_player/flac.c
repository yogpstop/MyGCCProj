#include <FLAC/stream_decoder.h>
#include <stdlib.h>
#include "main.h"
#define CHANNELS 2
#define BITS 16
static void ecb(const FLAC__StreamDecoder *d,
		FLAC__StreamDecoderErrorStatus s, void *c) {
}
static void mdcb(const FLAC__StreamDecoder *d,
		const FLAC__StreamMetadata *m, void *c) {
}
static FLAC__StreamDecoderWriteStatus wcb(const FLAC__StreamDecoder *d,
		const FLAC__Frame *f, const FLAC__int32* const b[], void *c) {
	int i;
	buf_str *ctx = c;
	for (i = 0; i < f->header.blocksize; i++) {
		if (ctx->to <= 0) return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
		ctx->to--;
		if (ctx->from > 0) { ctx->from--; continue; }
		uint16_t *ptr = ctx->p.buf + (ctx->cur_id * ctx->p.period + ctx->cur_period) * CHANNELS * BITS / 8;
		ptr[0] = b[0][i];
		ptr[1] = b[1][i];
		if (++ctx->cur_period >= ctx->p.period) {
			ctx->cur_id++;
			ctx->cur_id = ctx->cur_id & ctx->p.buf_max;
			MUTEX_LOCK(ctx->p.mutex + ctx->cur_id);
			MUTEX_UNLOCK(ctx->p.mutex + ((ctx->cur_id - 1) & ctx->p.buf_max));
			ctx->cur_period = 0;
		}
	}
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}
void flac_read(FILE *f, buf_str *ctx) {
	ctx->from /= CHANNELS * BITS / 8;
	ctx->to /= CHANNELS * BITS / 8;
	FLAC__StreamDecoder *d = FLAC__stream_decoder_new();
	FLAC__stream_decoder_init_FILE(d, f, wcb, mdcb, ecb, ctx);
	FLAC__stream_decoder_process_until_end_of_stream(d);
	FLAC__stream_decoder_delete(d);
}
