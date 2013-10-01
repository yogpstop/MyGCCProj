#pragma once

#include <time.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct getplayerstatus* getplayerstatus();
void freegetplayerstatus(struct getplayerstatus* gps);

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
	uint16_t ms_port;
	long ms_thread;
	char* category;
};

struct getpublishstatus* getpublishstatus();
void freegetpublishstatus(struct getpublishstatus* gps);

struct getpublishstatus {
	char* live_id;
	char* token;
	char* rtmp_url;
	char* rtmp_ticket;
	char* rtmp_stream;
	int bitrate;
};

#ifdef __cplusplus
}
#endif
