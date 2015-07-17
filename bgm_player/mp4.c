#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <neaacdec.h>
#include "main.h"
#define MKFCC(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))
#define _BE2LE4(be) (be << 24) | ((be & 0xFF00) << 8) | ((be >> 8) & 0xFF00) | (be >> 24)
#define BE2LE4(be) ((uint32_t)(_BE2LE4(((uint32_t)(be)))))
#define BITS 16
typedef struct {
	uint32_t *sample_size;
	struct sc_s {
		uint32_t first_chunk;
		uint32_t samples_per_chunk;
	} *sample_to_chunk;
	uint32_t *chunk_offset;
	void *es_descriptor;
	unsigned long esd_len;
	uint32_t ss_max;
	void *usr;
} user_s;
typedef struct {
	uint32_t fc;
	void (*hdl)(FILE*, size_t, user_s*);
} box;
static void do_stsc(FILE*, size_t, user_s*);
static void do_stco(FILE*, size_t, user_s*);
static void do_stsz(FILE*, size_t, user_s*);
static void do_esds(FILE*, size_t, user_s*);
static void do_mp4a(FILE*, size_t, user_s*);
static void do_stsd(FILE*, size_t, user_s*);
//static void do_hdlr(FILE*, size_t, user_s*);
static void do_trak(FILE*, size_t, user_s*);
static void do_box(FILE*, size_t, user_s*);
static const box boxes[] = {
	{MKFCC('m','o','o','v'), do_box},
	{MKFCC('t','r','a','k'), do_trak},
	{MKFCC('m','d','i','a'), do_box},
//	{MKFCC('h','d','l','r'), do_hdlr},
	{MKFCC('m','i','n','f'), do_box},
	{MKFCC('s','t','b','l'), do_box},
	{MKFCC('s','t','s','d'), do_stsd},
	{MKFCC('m','p','4','a'), do_mp4a},
	{MKFCC('e','s','d','s'), do_esds},
	{MKFCC('s','t','s','z'), do_stsz},
	{MKFCC('s','t','c','o'), do_stco},
	{MKFCC('s','t','s','c'), do_stsc},
	{0, NULL}
};
static void do_stsc(FILE *f, size_t max, user_s *user) {
	fseek(f, 4, SEEK_CUR);
	uint32_t count, i;
	fread(&count, 4, 1, f);
	count = BE2LE4(count);
	if (user->sample_to_chunk) free(user->sample_to_chunk);
	user->sample_to_chunk = malloc(sizeof(*user->sample_to_chunk) * (count + 1));
	for (i = 0; i < count; i++) {
		fread(user->sample_to_chunk + i, 4, 2, f);
		user->sample_to_chunk[i].first_chunk = 
				BE2LE4(user->sample_to_chunk[i].first_chunk);
		user->sample_to_chunk[i].samples_per_chunk = 
				BE2LE4(user->sample_to_chunk[i].samples_per_chunk);
		fseek(f, 4, SEEK_CUR);//FIXME
	}
	user->sample_to_chunk[i].first_chunk = 0;
	user->sample_to_chunk[i].samples_per_chunk = 0;
}
static void do_stco(FILE *f, size_t max, user_s *user) {
	fseek(f, 4, SEEK_CUR);
	uint32_t count, i;
	fread(&count, 4, 1, f);
	count = BE2LE4(count);
	if (user->chunk_offset) free(user->chunk_offset);
	user->chunk_offset = malloc(4 * (count + 1));
	fread(user->chunk_offset, 4, count, f);
	for (i = 0; i < count; i++)
		user->chunk_offset[i] = BE2LE4(user->chunk_offset[i]);
	user->chunk_offset[count] = 0;
}
static void do_stsz(FILE *f, size_t max, user_s *user) {
	fseek(f, 8, SEEK_CUR);//FIXME
	uint32_t count, i;
	user->ss_max = 0;
	fread(&count, 4, 1, f);
	count = BE2LE4(count);
	if (user->sample_size) free(user->sample_size);
	user->sample_size = malloc(4 * (count + 1));
	fread(user->sample_size, 4, count, f);
	for (i = 0; i < count; i++) {
		user->sample_size[i] = BE2LE4(user->sample_size[i]);
		if (user->ss_max < user->sample_size[i]) user->ss_max = user->sample_size[i];
	}
	user->sample_size[count] = 0;
}
static void do_esds(FILE *f, size_t max, user_s *user) {
	uint8_t len;
	//FIXME max > 0x7F
	fseek(f, 25, SEEK_CUR);//FIXME
	fread(&len, 1, 1, f);
	if (user->es_descriptor) free(user->es_descriptor);
	user->esd_len = len;
	user->es_descriptor = malloc(len);
	fread(user->es_descriptor, len, 1, f);
	fseek(f, max - 26 - len, SEEK_CUR);
}
static void do_mp4a(FILE *f, size_t max, user_s *user) {
	fseek(f, 28, SEEK_CUR);
	do_box(f, max - 28, user);//FIXME
}
static void do_stsd(FILE *f, size_t max, user_s *user) {
	fseek(f, 8, SEEK_CUR);
	do_box(f, max - 8, user);//FIXME
}
/*static void do_hdlr(FILE *f, size_t max, user_s *user) {
	uint32_t type;
	fseek(f, 8, SEEK_CUR);
	fread(&type, 4, 1, f);
	fseek(f, max - 12, SEEK_CUR);
}*/
static void do_trak(FILE *f, size_t max, user_s *user) {
	do_box(f, max, user);
	if (user->es_descriptor && user->sample_size && user->sample_to_chunk && user->chunk_offset) {
		long saved_pos =ftell(f);
		uint32_t r_sample, r_chunk;
		void *tmp = malloc(user->ss_max), *dest;
		uint32_t *sz = user->sample_size;
		uint32_t *co = user->chunk_offset;
		struct sc_s *sc = user->sample_to_chunk;
		//FIXME sc->first_chunk != 1
		unsigned long sr; unsigned char ch;
		NeAACDecHandle aac = NeAACDecOpen();
		NeAACDecConfigurationPtr conf = NeAACDecGetCurrentConfiguration(aac);
		//FIXME conf->defSampleRate != 44100
		conf->outputFormat = FAAD_FMT_16BIT;
		NeAACDecSetConfiguration(aac, conf);
		NeAACDecInit2(aac, user->es_descriptor, user->esd_len, &sr, &ch);
		NeAACDecFrameInfo inf;
		while (sc->first_chunk) {
			if(sc[1].first_chunk) r_chunk = sc[1].first_chunk - sc->first_chunk;
			else r_chunk = 0xFFFFFFFF;//UINT32_MAX
			while (*co && r_chunk) {
				r_sample = sc->samples_per_chunk;
				fseek(f, *co, SEEK_SET);
				while (*sz && r_sample) {
					fread(tmp, *sz, 1, f);
					dest = NeAACDecDecode(aac, &inf, tmp, *sz);
					//FIXME inf.bytesconsumed != *sz
					bytes2buf(dest, inf.samples * BITS / 8, user->usr);
					sz++; r_sample--;
				}
				co++; r_chunk--;
			}
			sc++;
		}
		NeAACDecClose(aac);
		free(tmp);
		free(user->es_descriptor);
		user->es_descriptor = NULL;
		fseek(f, saved_pos, SEEK_SET);
	}
}
static void do_box(FILE *f, size_t max, user_s *user) {
	static uint32_t buf[2];
	uint32_t len;
	while (max) {
		fread(buf, 8, 1, f);
		if (feof(f)) break;
		if (!buf[0]) len = max;
		// FIXME else if (buf[0] == 0x01000000) unsupported
		else len = BE2LE4(buf[0]);
		max -= len;
		len -= 8;
		const box *cur = boxes;
		do { if (buf[1] == cur->fc) {
			cur->hdl(f, len, user);
			break;
		} } while((++cur)->fc);
		if (!cur->fc) fseek(f, len, SEEK_CUR);
	}
}
void mp4_read(FILE *f, buf_str *ctx) {
	user_s str = {};
	str.usr = ctx;
	do_box(f, 0xFFFFFFFF, &str);
	fclose(f);
}
