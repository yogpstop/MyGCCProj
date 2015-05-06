#pragma once

#include <time.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STRUCT_SET_STR(str, var); if (!((str*)user)->var) { \
		((str*)user)->var = malloc(strlen(el_v) + 1); \
		if (((str*)user)->var) strcpy(((str*)user)->var, el_v); }
#define STRUCT_SET_INT(str, var); \
		((str*)user)->var = strtoul(el_v, NULL, 10);
#define STRUCT_SAFE_FREE(str, var); \
		if (str->var) { free(str->var); str->var = NULL; }

#define GBS_STR(var); STRUCT_SET_STR(struct getpublishstatus, var);
#define GBS_INT(var); STRUCT_SET_INT(struct getpublishstatus, var);
#define IF_GBS_FREE(var); STRUCT_SAFE_FREE(gbs, var);

#define GPS_STR(var); STRUCT_SET_STR(struct getplayerstatus, var);
#define GPS_INT(var); STRUCT_SET_INT(struct getplayerstatus, var);
#define GPS_LNG(var); STRUCT_SET_INT(struct getplayerstatus, var);
#define IF_GPS_FREE(var); STRUCT_SAFE_FREE(gps, var);

struct getplayerstatus* getplayerstatus(char*);
void freegetplayerstatus(struct getplayerstatus*);

struct getplayerstatus {
	char* live_id;
	char* title;
	char* description;
	char* provider_type;
	char* community_id;
	int owner_id;
	char* owner_name;
	int is_reserved;
	int watch_count;
	int comment_count;
	time_t base_time;
	time_t open_time;
	time_t start_time;
	time_t end_time;
	char* twitter_tag;
	int archive;
	char* content1;
	char* room_label;
	int room_seetno;
	char* rtmp_url;
	char* rtmp_ticket;
	char* ms_addr;
	char* ms_port;
	char* ms_thread;
	char* category;
};

struct getpublishstatus* getpublishstatus();
void freegetpublishstatus(struct getpublishstatus*);

struct getpublishstatus {
	char* live_id;
	char* token;
	char* rtmp_url;
	char* rtmp_ticket;
	char* rtmp_stream;
	int bitrate;
};

struct getthreads* getthreads(char*);
void freegetthreads(struct getthreads*);

struct ms_thread {
	char* host;
	char* port;
	char* tid;
};

struct getthreads {
	char* com;
	int l;
	struct ms_thread* t;
};

void getcomment(char*, char*, char*);

struct chat {
	uint32_t no;
	int64_t vpos;
	time_t date;
	uint64_t date_usec;
	char *mail;
	char *user_id;
	int anonymity;
	int premium;
	int scores;
	char *name;
};

#ifdef __cplusplus
}
#endif
