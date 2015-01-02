#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "osu.h"
#include "curve.h"

static inline char *readfully(const char *n) {
	int fd = open(n, O_RDONLY);
	struct stat s;
	fstat(fd, &s);
	char *m = malloc(s.st_size + 1), *p = m;
	int r = s.st_size, t;
	while((t = read(fd, p, r)) > 0) {
		r -= t; p += t; }
	*p = 0;
	close(fd);
	return m;
}
#define FPUT(arg) comma = strtok_r(*line, ",", &s_comma); \
		if (!comma) continue; r[c].arg = strtol(comma, &comma, 10);
#define PUT(arg) comma = strtok_r(NULL, ",", &s_comma); \
		if (!comma) continue; r[c].arg = strtol(comma, &comma, 10);
#define PUTD(arg) comma = strtok_r(NULL, ",", &s_comma); \
		if (!comma) continue; r[c].arg = strtod(comma, &comma);

static inline hito *readHits(char **line, char **s_line, unsigned int *al) {
	unsigned int rl = 256, c = 0;
	hito *r = malloc(rl * sizeof(hito));
	for (; (*line = strtok_r(NULL, "\r\n", s_line)) && **line != '['; c++) {
		if (c >= rl) { rl *= 2; r = realloc(r, rl * sizeof(hito)); }
		r[c].p = malloc(sizeof(point));
		r[c].plen = 1;
		r[c].slid_type = ' ';
		r[c].alpha = -1;
		r[c].clicked = SIZE_MAX;
		r[c].cpos = 0;
		r[c].acsize = -1;
		char *s_comma, *comma;
		FPUT(p->x);
		PUT(p->y);
		PUT(time);
		PUT(type);
		PUT(sound);
		if (r[c].type & OSU_OBJ_TYPE_SPIN) {
			PUT(endtime)
			r[c].repeat = 1;
			r[c].length = r[c].endtime - r[c].time;
		} else if (r[c].type & OSU_OBJ_TYPE_SLIDER) {
			comma = strtok_r(NULL, ",", &s_comma);
			if (!comma) continue;
			unsigned int new = 1;
			char *p = comma;
			while (*p) {
				if (*p == '|') new++; p++; }
			r[c].plen = new;
			r[c].p = realloc(r[c].p, sizeof(point) * r[c].plen);
			char *s_vert, *vert;
			vert = strtok_r(comma, "|", &s_vert);
			if (!vert) goto ccnt;
			r[c].slid_type = *vert;
			new = 1;
			while ((vert = strtok_r(NULL, "|", &s_vert))) {
				char *colon, *s_colon;
				colon = strtok_r(vert, ":", &s_colon);
				if (!colon) continue;
				r[c].p[new].x = strtol(colon, &colon, 10);
				colon = strtok_r(NULL, ":", &s_colon);
				if (!colon) continue;
				r[c].p[new].y = strtol(colon, &colon, 10);
				new++;
			}
ccnt:
			PUT(repeat)//TODO
			PUTD(length)
		} else {
			r[c].repeat = 1;
			r[c].length = 0;
			r[c].endtime = r[c].time;
		}
	}
	*al =  c;
	return r;
}

static inline timing *readTims(char **line, char **s_line, unsigned int *al) {
	unsigned int rl = 16, c = 0;
	timing *r = malloc(rl * sizeof(timing));
	double prev = 0;
	for (; (*line = strtok_r(NULL, "\r\n", s_line)) && **line != '['; c++) {
		if (c >= rl) { rl *= 2; r = realloc(r, rl * sizeof(timing)); }
		char *s_comma, *comma;
		FPUT(off);
		PUTD(beat_len);
		if (r[c].beat_len < 0) r[c].beat_len = prev;
		prev = r[c].beat_len;
		PUT(meter);
		PUT(stype);
		PUT(sset);
		PUT(vol);
		PUT(reset);
		PUT(kiai);
	}
	*al =  c;
	return r;
}

static inline void readDiff(char **line, char **s_line, difficulty *r) {
	r->sm = 1;
	r->st = 1;
	r->ar = 6;
	while ((*line = strtok_r(NULL, "\r\n", s_line)) && **line != '[') {
		char *colon, *s_colon, *key;
		key = strtok_r(*line, ": ", &s_colon);
		colon = strtok_r(NULL, ": ", &s_colon);
		if (!strcmp(key, "HPDrainRate"))
			r->hp = strtod(colon, &colon);
		else if (!strcmp(key, "CircleSize"))
			r->cs = strtod(colon, &colon);
		else if (!strcmp(key, "OverallDifficulty"))
			r->od = strtod(colon, &colon);
		else if (!strcmp(key, "ApproachRate"))
			r->ar = strtod(colon, &colon);
		else if (!strcmp(key, "SliderMultiplier"))
			r->sm = strtod(colon, &colon);
		else if (!strcmp(key, "SliderTickRate"))
			r->st = strtod(colon, &colon);
	}
}

static inline void readGen(char **line, char **s_line, char **afn) {
	while ((*line = strtok_r(NULL, "\r\n", s_line)) && **line != '[') {
		char *colon, *s_colon, *key;
		key = strtok_r(*line, ":", &s_colon);
		colon = strtok_r(NULL, ":", &s_colon);
		if (!colon) continue;
		if (*colon == ' ') colon++;
		if (!strcmp(key, "AudioFilename"))
			strcpy(*afn = malloc(strlen(colon) + 1), colon);
	}
}

void readosu(const char *n, hito **hit, unsigned int *hlen,
		timing **tim, unsigned int *tlen, difficulty *diff, char **afn) {
	char * const d = readfully(n);
	char *s_line;
	char *line = strtok_r(d, "\r\n", &s_line);
	while (line) {
		if (!strcmp(line, "[HitObjects]"))
			*hit = readHits(&line, &s_line, hlen);
		else if (!strcmp(line, "[TimingPoints]"))
			*tim = readTims(&line, &s_line, tlen);
		else if (!strcmp(line, "[Difficulty]"))
			readDiff(&line, &s_line, diff);
		else if (!strcmp(line, "[General]"))
			readGen(&line, &s_line, afn);
		else
			line = strtok_r(NULL, "\r\n", &s_line);
	}
	hito *cur, *max = *hit + *hlen;
	timing *ctp = *tim, *mtp = *tim + *tlen - 1;
	for (cur = *hit; cur < max; cur++) {
		while (ctp < mtp && cur->time >= ctp[1].off) ctp++;
		if (!(cur->type & OSU_OBJ_TYPE_SLIDER)) continue;
		cur->length = cur->length / (diff->sm * 100) * ctp->beat_len;
		point *tmp = NULL;
		unsigned int tmp_len;
		switch (cur->slid_type) {
			case OSU_OBJ_CUR_PASS:
				tmp = pass   (cur->plen, cur->p, &tmp_len);
				break;
			case OSU_OBJ_CUR_BEZ:
				tmp = bezier (cur->plen, cur->p, &tmp_len);
				break;
			case OSU_OBJ_CUR_LIN:
				tmp = linear (cur->plen, cur->p, &tmp_len);
				break;
			case OSU_OBJ_CUR_CAT:
				tmp = catmull(cur->plen, cur->p, &tmp_len);
				break;
		}
		if (tmp) {
			polygon(tmp_len, tmp, diff->cs, 100000, &cur->side);
			free(cur->p);
			cur->plen = cur->length / 4;
			cur->p = malloc(cur->plen * sizeof(point));
			uniform(tmp_len, tmp, cur->plen, cur->p);
			free(tmp);
		}
		cur->endtime = cur->time + cur->length * cur->repeat;
	}
	free(d);
}
