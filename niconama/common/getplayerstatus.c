#include "main.h"
#include "xml_struct.h"
#include "xml.h"
#include "cookie.h"

static void callback3(void *user, char *el_n, char *el_v) {
	if(!strcmp(el_n,"getplayerstatus/stream/id")){
		GPS_STR(live_id);
	} else if(!strcmp(el_n,"getplayerstatus/stream/title")){
		GPS_STR(title);
	} else if(!strcmp(el_n,"getplayerstatus/stream/description")){
		GPS_STR(description);
	} else if(!strcmp(el_n,"getplayerstatus/stream/provider_type")){
		GPS_STR(provider_type);
	} else if(!strcmp(el_n,"getplayerstatus/stream/default_community")){
		GPS_STR(community_id);
	} else if(!strcmp(el_n,"getplayerstatus/stream/owner_name")){
		GPS_STR(owner_name);
	} else if(!strcmp(el_n,"getplayerstatus/stream/twitter_tag")){
		GPS_STR(twitter_tag);
	} else if(!strcmp(el_n,"getplayerstatus/stream/contents_list/contents")){
		GPS_STR(content1);
	} else if(!strcmp(el_n,"getplayerstatus/user/room_label")){
		GPS_STR(room_label);
	} else if(!strcmp(el_n,"getplayerstatus/rtmp/url")){
		GPS_STR(rtmp_url);
	} else if(!strcmp(el_n,"getplayerstatus/rtmp/ticket")){
		GPS_STR(rtmp_ticket);
	} else if(!strcmp(el_n,"getplayerstatus/ms/addr")){
		GPS_STR(ms_addr);
	} else if(!strcmp(el_n,"getplayerstatus/ms/port")){
		GPS_STR(ms_port);
	} else if(!strcmp(el_n,"getplayerstatus/ms/thread")){
		GPS_STR(ms_thread);
	} else if(!strcmp(el_n,"getplayerstatus/marquee/category")){
		GPS_STR(category);
	} else if(!strcmp(el_n,"getplayerstatus/stream/owner_id")){
		GPS_INT(owner_id);
	} else if(!strcmp(el_n,"getplayerstatus/stream/is_reserved")){
		GPS_INT(is_reserved);
	} else if(!strcmp(el_n,"getplayerstatus/stream/watch_conut")){
		GPS_INT(watch_count);
	} else if(!strcmp(el_n,"getplayerstatus/stream/comment_count")){
		GPS_INT(comment_count);
	} else if(!strcmp(el_n,"getplayerstatus/stream/archive")){
		GPS_INT(archive);
	} else if(!strcmp(el_n,"getplayerstatus/user/room_seetno")){
		GPS_INT(room_seetno);
	} else if(!strcmp(el_n,"getplayerstatus/stream/base_time")){
		GPS_LNG(base_time);
	} else if(!strcmp(el_n,"getplayerstatus/stream/open_time")){
		GPS_LNG(open_time);
	} else if(!strcmp(el_n,"getplayerstatus/stream/start_time")){
		GPS_LNG(start_time);
	} else if(!strcmp(el_n,"getplayerstatus/stream/end_time")){
		GPS_LNG(end_time);
	}
}

void freegetplayerstatus(struct getplayerstatus* gps) {
	IF_GPS_FREE(live_id);
	IF_GPS_FREE(title);
	IF_GPS_FREE(description);
	IF_GPS_FREE(provider_type);
	IF_GPS_FREE(community_id);
	IF_GPS_FREE(owner_name);
	IF_GPS_FREE(twitter_tag);
	IF_GPS_FREE(content1);
	IF_GPS_FREE(room_label);
	IF_GPS_FREE(rtmp_url);
	IF_GPS_FREE(rtmp_ticket);
	IF_GPS_FREE(ms_addr);
	IF_GPS_FREE(ms_port);
	IF_GPS_FREE(ms_thread);
	IF_GPS_FREE(category);
	free(gps);
}

struct getplayerstatus* getplayerstatus(char* liveid){
  int sock = create_socket("live.nicovideo.jp", "http", 0);
  send(sock, "GET /api/getplayerstatus?v=lv", 29, 0);
  send(sock, liveid, strlen(liveid), 0);
  send(sock, " HTTP/1.1\r\n"
      "Host: live.nicovideo.jp\r\nCookie: user_session=", 57, 0);
  char *session = getSession();
  send(sock, session, strlen(session), 0);
  send(sock, "\r\n\r\n", 4, 0);
  struct getplayerstatus* gps = (struct getplayerstatus*)
      malloc(sizeof(struct getplayerstatus));
  memset(gps, 0, sizeof(struct getplayerstatus));
  struct xml data;
  memset(&data, 0, sizeof(struct xml));
  data.tag = callback3;
  data.user = gps;
  char recvdata;
  int http = 0;
  while (1) {
    if (1 > recv(sock, &recvdata, 1, 0))
      break;
    if (1 < http)
      xml_next(recvdata, &data);
    else if (recvdata == '\n')
      http++;
    else if (recvdata != '\r')
      http = 0;
  }
  close(sock);
  return gps;
}
/*
int main(int argc, char **argv) {
	WS2I
	struct getplayerstatus *gps = getplayerstatus(argv[1]);
	printf("%s %s %s %s %s %d %s %d %d %d %I64d %I64d %I64d %I64d %s %d %s %s %d %s %s %s %s %s %s", gps->live_id, gps->title, gps->description, gps->provider_type, gps->community_id, gps->owner_id, gps->owner_name, gps->is_reserved, gps->watch_count, gps->comment_count, gps->base_time, gps->open_time, gps->start_time, gps->end_time, gps->twitter_tag, gps->archive, gps->content1, gps->room_label, gps->room_seetno, gps->rtmp_url, gps->rtmp_ticket, gps->ms_addr, gps->ms_port, gps->ms_thread, gps->category);
    freegetplayerstatus(gps);
	WS2U
	return 0;
}*/
