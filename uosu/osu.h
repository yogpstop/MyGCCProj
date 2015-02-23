#ifndef _UOSU_OSU_H
#define _UOSU_OSU_H
#include "curve.h"

#define OSU_OBJ_TYPE_NORMAL 1
#define OSU_OBJ_TYPE_SLIDER 2
#define OSU_OBJ_TYPE_NEW    4
#define OSU_OBJ_TYPE_SPIN   8
#define OSU_OBJ_SND_NONE    0
#define OSU_OBJ_SND_NORM    1
#define OSU_OBJ_SND_WHIS    2
#define OSU_OBJ_SND_FIN     4
#define OSU_OBJ_SND_CLAP    8
#define OSU_OBJ_CUR_PASS   'P'
#define OSU_OBJ_CUR_BEZ    'B'
#define OSU_OBJ_CUR_LIN    'L'
#define OSU_OBJ_CUR_CAT    'C'

typedef struct {
	double hp;
	double cs;
	double od;
	double ar;
	double sm;
	double st;
} difficulty;
typedef struct {
	unsigned int off;
	double beat_len;
	unsigned int meter;
	unsigned int stype;
	unsigned int sset;
	unsigned int vol;
	unsigned int reset;
	unsigned int kiai;

} timing;
typedef struct {
	double r;
	double g;
	double b;
} color;
typedef struct {
	color combo[8];
	color border;
} colours;
typedef struct {
	char slid_type;
	unsigned int plen;
	point *p;
	size_t time;
	unsigned int type;
	unsigned int sound;
	unsigned int repeat;
	double length;
	size_t endtime;
	unsigned int crep;
	unsigned int cpos;
	double alpha;
	double rgb;
	double acsize;
	size_t clicked;
	double cshow;
	color *combo;
} hito;
#endif
