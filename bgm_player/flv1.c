#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mpg123.h>
#include "main.h"

static int callback(uint8_t type, uint32_t ts, unsigned int len, void *buf, void *c) {
	buf_str *ctx = ((void**)c)[0];
	unsigned char *dest; size_t done;
	if (type != 8) return 0;
	if (((uint8_t*)buf)[0] == 0x2F) {
		mpg123_feed(((void**)c)[1], buf + 1, len - 1);
		while (1) {
			mpg123_framebyframe_next(((void**)c)[1]); //FIXME == MPG123_NEW_FORMAT
			mpg123_framebyframe_decode(((void**)c)[1], NULL, &dest, &done);
			if (!done) break;
			bytes2buf(dest, done, ctx);
			if (ctx->to <= 0) break;
		}
	}
	return 0;
}
void flv1_read(FILE *f, buf_str *ctx) {
	void *a[2];
	a[0] = ctx;
	mpg123_init();
	a[1] = mpg123_new(NULL, NULL);
	mpg123_open_feed(a[1]);
	flv_per_tag(f, a, callback);
	mpg123_close(a[1]);
	mpg123_delete(a[1]);
	mpg123_exit();
}
