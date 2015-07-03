#include "main.h"
#include "n_sock.h"
#include "cookie.h"
#include "xml_struct.h"
#include "xml.h"

static void callback3(void *user, char *el_n, char *el_v) {
	if(!strcmp(el_n,"getpublishstatus/stream/id")){
		GBS_STR(live_id);
	} else if(!strcmp(el_n,"getpublishstatus/stream/token")){
		GBS_STR(token);
	} else if(!strcmp(el_n,"getpublishstatus/rtmp/url")){
		GBS_STR(rtmp_url);
	} else if(!strcmp(el_n,"getpublishstatus/rtmp/ticket")){
		GBS_STR(rtmp_ticket);
	} else if(!strcmp(el_n,"getpublishstatus/rtmp/stream")){
		GBS_STR(rtmp_stream);
	} else if(!strcmp(el_n,"getpublishstatus/rtmp/bitrate")){
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
  int sock = create_socket("live.nicovideo.jp", "http", 0);
  send(sock, "GET /api/getpublishstatus HTTP/1.1\r\n"
      "Host: live.nicovideo.jp\r\nCookie: user_session=", 82, 0);
  char *session = getSession();
  send(sock, session, strlen(session), 0);
  send(sock, "\r\n\r\n", 4, 0);
  struct getpublishstatus* gbs = (struct getpublishstatus*)
      malloc(sizeof(struct getpublishstatus));
  memset(gbs, 0, sizeof(struct getpublishstatus));
  struct xml data;
  memset(&data, 0, sizeof(struct xml));
  data.tag = callback3;
  data.user = gbs;
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
  CLOSESOCKET(sock);
  return gbs;
}

int main() {
	WS2I
	struct getpublishstatus *gbs = getpublishstatus();
	printf("%s %s %s %s %s %d", gbs->live_id, gbs->token, gbs->rtmp_url, gbs->rtmp_ticket, gbs->rtmp_stream, gbs->bitrate);
    freegetpublishstatus(gbs);
	WS2U
	return 0;
}
