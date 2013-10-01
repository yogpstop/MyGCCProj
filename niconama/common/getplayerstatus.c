#include "main.h"
#include "xml_struct.h"
#include "xml.h"
#include "cookie.h"

#define GPS_STR(var); if(!((struct getplayerstatus*)data->user)->var) { \
			((struct getplayerstatus*)data->user)->var = \
			(char*) malloc(strlen(data->el_v)+1); \
			if(((struct getplayerstatus*)data->user)->var) \
				strcpy(((struct getplayerstatus*)data->user)->var,data->el_v); \
		}
#define GPS_INT(var); ((struct getplayerstatus*)data->user)->var = atoi(data->el_v);

#define GPS_LNG(var); ((struct getplayerstatus*)data->user)->var = atol(data->el_v);
		
#define IF_GPS_FREE(var); if(gps->var) { \
			free(gps->var); \
		}

static void callback3(struct xml *data) {
	if(!strcmp(data->el_n,"getplayerstatus.stream.id")){
		GPS_STR(live_id);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.title")){
		GPS_STR(title);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.description")){
		GPS_STR(description);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.provider_type")){
		GPS_STR(provider_type);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.default_community")){
		GPS_STR(community_id);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.owner_name")){
		GPS_STR(owner_name);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.twitter_tag")){
		GPS_STR(twitter_tag);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.contents_list.contents")){
		GPS_STR(content1);
	} else if(!strcmp(data->el_n,"getplayerstatus.user.room_label")){
		GPS_STR(room_label);
	} else if(!strcmp(data->el_n,"getplayerstatus.rtmp.url")){
		GPS_STR(rtmp_url);
	} else if(!strcmp(data->el_n,"getplayerstatus.rtmp.ticket")){
		GPS_STR(rtmp_ticket);
	} else if(!strcmp(data->el_n,"getplayerstatus.ms.addr")){
		GPS_STR(ms_addr);
	} else if(!strcmp(data->el_n,"getplayerstatus.marquee.category")){
		GPS_STR(category);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.owner_id")){
		GPS_INT(owner_id);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.is_reserved")){
		GPS_INT(is_reserved);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.watch_conut")){
		GPS_INT(watch_count);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.comment_count")){
		GPS_INT(comment_count);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.archive")){
		GPS_INT(archive);
	} else if(!strcmp(data->el_n,"getplayerstatus.user.room_seetno")){
		GPS_INT(room_seetno);
	} else if(!strcmp(data->el_n,"getplayerstatus.ms.port")){
		GPS_INT(ms_port);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.base_time")){
		GPS_LNG(base_time);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.open_time")){
		GPS_LNG(open_time);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.start_time")){
		GPS_LNG(start_time);
	} else if(!strcmp(data->el_n,"getplayerstatus.stream.end_time")){
		GPS_LNG(end_time);
	} else if(!strcmp(data->el_n,"getplayerstatus.ms.thread")){
		GPS_LNG(ms_thread);
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
	IF_GPS_FREE(category);
	free(gps);
}

struct getplayerstatus* getplayerstatus(char* liveid){
  int sock;
  {
    struct addrinfo *res;
    static char thread[200] = "GET /api/getplayerstatus?v=lv";
    strcat(thread, liveid);
    strcat(thread, " HTTP/1.1\r\nHost: live.nicovideo.jp\r\nCookie: user_session=");
    strcat(thread, user_session);
    strcat(thread, "\r\n\r\n");
    getaddrinfo("live.nicovideo.jp", "http", NULL, &res);
    sock = socket(res->ai_family, SOCK_STREAM, 0);
    connect(sock, res->ai_addr, res->ai_addrlen);
    send(sock, thread, strlen(thread), 0);
    freeaddrinfo(res);
  }
  
  char recvdata;
  ssize_t recvlen;
  
  struct xml data;
  struct getplayerstatus* gps = (struct getplayerstatus*)
									malloc(sizeof(struct getplayerstatus));
  memset(&data,0,sizeof(struct xml));
  memset(gps,0,sizeof(struct getplayerstatus));
  data.tag=callback3;
  data.user=gps;

  int http=0;
  while(1) {
    recvlen = recv(sock, &recvdata, 1, 0);
    if(recvlen < 1)
      break;
    if(http>1)
      next(recvdata,&data);
    else if(recvdata == '\n')
      http++;
    else if(recvdata != '\r')
      http = 0;
  }
  close(sock);
  return gps;
}
