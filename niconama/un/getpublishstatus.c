#include "main.h"
#include "xml_struct.h"
#include "xml.h"

#define GBS_STR(var); if(!((struct getpublishstatus*)data->user)->var) { \
			((struct getpublishstatus*)data->user)->var = malloc(strlen(data->el_v)+1); \
			if(((struct getpublishstatus*)data->user)->var) \
				strcpy(((struct getpublishstatus*)data->user)->var,data->el_v); \
		}
#define GBS_INT(var); ((struct getpublishstatus*)data->user)->var = atoi(data->el_v);
		
#define IF_GBS_FREE(var); if(gbs->var) { \
			free(gbs->var); \
		}

extern char *user_session;

static void callback3(struct xml *data) {
	if(!strcmp(data->el_n,"getpublishstatus.stream.id")){
		GBS_STR(live_id);
	} else if(!strcmp(data->el_n,"getpublishstatus.stream.token")){
		GBS_STR(token);
	} else if(!strcmp(data->el_n,"getpublishstatus.rtmp.url")){
		GBS_STR(rtmp_url);
	} else if(!strcmp(data->el_n,"getpublishstatus.rtmp.ticket")){
		GBS_STR(rtmp_ticket);
	} else if(!strcmp(data->el_n,"getpublishstatus.rtmp.stream")){
		GBS_STR(rtmp_stream);
	} else if(!strcmp(data->el_n,"getpublishstatus.rtmp.bitrate")){
		GBS_INT(bitrate);
	}
}

void freegetpublishstatus(struct getpublishstatus* gbs) {
	IF_GBS_FREE(live_id);
	IF_GBS_FREE(token);
	IF_GBS_FREE(rtmp_url);
	IF_GBS_FREE(rtmp_ticket);
	IF_GBS_FREE(rtmp_stream);
	free(gbs);
}

struct getpublishstatus* getpublishstatus(){
  int sock;
  {
    struct addrinfo *res;
    static char thread[200] = "GET /api/getpublishstatus HTTP/1.1\r\nHost: live.nicovideo.jp\r\nCookie: user_session=";
    strcat(thread, user_session);
    strcat(thread, "\r\n\r\n");
    getaddrinfo("live.nicovideo.jp", "http", NULL, &res);
    sock = socket(res->ai_family, SOCK_STREAM, 0);
    connect(sock, res->ai_addr, res->ai_addrlen);
    send(sock, thread, strlen(thread), 0);
    freeaddrinfo(res);
  }
  
  uint8_t recvdata;
  ssize_t recvlen;
  
  struct xml data;
  struct getpublishstatus* gbs = malloc(sizeof(struct getpublishstatus));
  memset(&data,0,sizeof(struct xml));
  memset(gbs,0,sizeof(struct getpublishstatus));
  data.tag=callback3;
  data.user=gbs;

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
  return gbs;
}