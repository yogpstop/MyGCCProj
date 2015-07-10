#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flv_core.h"
typedef struct flv_tag {
	uint8_t type;
	uint32_t ts;
	unsigned int len;
	void *dat;
	struct flv_tag *prev;
	struct flv_tag *next;
} flv_tag;
static flv_tag *cur_tag = NULL;
static uint32_t offset = 0;
static int callback(uint8_t type, uint32_t ts, unsigned int len, void *dat) {
	if (type != 8 && type != 9) { fprintf(stderr,"WARNING: invalid TagType %2X\n", type); return 0; }
	if (!len) { fprintf(stderr, "WARNING: invalid DataSize\n"); return 0; }
	flv_tag *new = malloc(sizeof(flv_tag));
	new->type = type; new->ts = ts + offset; new->len = len; new->dat = dat;
	if (cur_tag) { cur_tag->next = new; new->prev = cur_tag; }
	else { new->prev = NULL; }
	cur_tag = new;
	return 1;
}
int main(int argc, char **argv) {
	int i;
	for (i = 4; i < argc; i++) {
		flv_per_tag(argv[i], callback);
		offset = cur_tag->ts + 21;//FIXME
	}
	FILE *fp = fopen(argv[1], "wb");
	flv_write_header(fp, 5);
	uint32_t min = strtoul(argv[2], NULL ,10), max = strtoul(argv[3], NULL, 10);
	void *last_sh_audio = NULL, *last_sh_video = NULL;
	unsigned int last_sh_audio_len, last_sh_video_len;
	int need = 0;
	while (cur_tag->prev) cur_tag = cur_tag->prev;
	while (cur_tag->ts < max) {
		need &= ~1;
		if (       (cur_tag->type == 8 && (((uint8_t*)cur_tag->dat)[0] & 0xF0) == 0xA0 && ((uint8_t*)cur_tag->dat)[1] == 0)
				|| (cur_tag->type == 9 && (((uint8_t*)cur_tag->dat)[0] & 0x0F) == 0x07 && ((uint8_t*)cur_tag->dat)[1] == 0)) {
			if (cur_tag->type == 8) {
				if (!last_sh_audio || last_sh_audio_len != cur_tag->len || memcmp(last_sh_audio, cur_tag->dat, last_sh_audio_len)) {
					fprintf(stderr, "New audio sequence header at %u\n", cur_tag->ts);
					last_sh_audio = cur_tag->dat;
					last_sh_audio_len = cur_tag->len;
					need |= 1;
				}
			} else if (cur_tag->type == 9) {
				if (!last_sh_video || last_sh_video_len != cur_tag->len || memcmp(last_sh_video, cur_tag->dat, last_sh_video_len)) {
					fprintf(stderr, "New video sequence header at %u\n", cur_tag->ts);
					last_sh_video = cur_tag->dat;
					last_sh_video_len = cur_tag->len;
					need |= 1;
				}
			}
			if (need & 1) flv_write_tag(fp, cur_tag->type, cur_tag->ts > min ? cur_tag->ts - min : 0, cur_tag->len, cur_tag->dat);
		} else {
			if (cur_tag->ts > min) {
				if (!(need & 2) && cur_tag->type == 9 && (((uint8_t*)cur_tag->dat)[0] & 0x0F) == 0x07) {
					if (((uint8_t*)cur_tag->dat)[0] != 0x17) 
					     need |= 1;
					else need |= 2;
				}
				if (!(need & 1)) {
					if (       cur_tag->type == 9 && !(need & 4)) {
						fprintf(stderr, "First video tag at %u\n", cur_tag->ts);
						need |= 4;
					} else if (cur_tag->type == 8 && !(need & 8)) {
						fprintf(stderr, "First audio tag at %u\n", cur_tag->ts);
						need |= 8;
					}
					flv_write_tag(fp, cur_tag->type, cur_tag->ts - min, cur_tag->len, cur_tag->dat);
				}
			}
		}
		if (!cur_tag->next) break;
		cur_tag = cur_tag->next;
	}
	while (cur_tag->prev) cur_tag = cur_tag->prev;
	while (cur_tag->next) { free(cur_tag->dat); cur_tag = cur_tag->next; free(cur_tag->prev); }
	free(cur_tag->dat);
	free(cur_tag);
	fclose(fp);
	return 0;
}
