#include <FLAC/stream_decoder.h>
#include <stdlib.h>
static void *data, *ptr;
static int len, rem, channels, bps;
static size_t gf, gt;
static void mdcb(const FLAC__StreamDecoder *d,
		const FLAC__StreamMetadata *m, void *c) {
	if (m->type == FLAC__METADATA_TYPE_STREAMINFO) {
		channels = m->data.stream_info.channels;
		bps = m->data.stream_info.bits_per_sample / 8;
		rem = len = m->data.stream_info.total_samples * channels * bps;
		data = ptr = malloc(len); // TODO: allocation failed
	}
}
static FLAC__StreamDecoderWriteStatus wcb(const FLAC__StreamDecoder *d,
		const FLAC__Frame *f, const FLAC__int32* const b[], void *c) {
	int i, j, k;
	for(i = 0; i < f->header.blocksize; i++) {
		if (gf > 0) {
			gf--;
			continue;
		}
		gt--;
		if (gt < 0) {
			break;
		}
		for(j = 0; j < channels; j++) {
			unsigned char *from = (void*)&(b[j][i]);
			unsigned char *to = ptr;
			for(k = 0; k < bps; k++) {
				*to++ = *from++;
				ptr++;
				rem--;
			}
		}
	}
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}
static void ecb(const FLAC__StreamDecoder *d, FLAC__StreamDecoderErrorStatus s,
		void *c) {
}
void *flac_read(FILE *f, int *size, size_t from, size_t to) {
	gf = from / 4;
	gt = to / 4;
	FLAC__StreamDecoder *d = FLAC__stream_decoder_new();
	FLAC__stream_decoder_init_FILE(d, f, wcb, mdcb, ecb, NULL);
	FLAC__stream_decoder_process_until_end_of_stream(d);
	FLAC__stream_decoder_delete(d);
	*size = len - rem;
	return data;
}
