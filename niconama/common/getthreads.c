#include "main.h"
#include "n_sock.h"
#include "cookie.h"
#include "xml_struct.h"
#include "xml.h"

static void callback3(void *user, char *el_n, char *el_v) {
	struct getthreads *gts = user;
	if(!strcmp(el_n,"getthreads/thread")){
		gts->t = realloc(gts->t, ++gts->l * sizeof(struct ms_thread));
		memset(gts->t + gts->l - 1, 0, sizeof(struct ms_thread));
	} else if(!strcmp(el_n,"getthreads/thread/server/socket_host")){
		if (gts->t[gts->l - 1].host) return;
		gts->t[gts->l - 1].host = malloc(strlen(el_v) + 1);
		strcpy(gts->t[gts->l - 1].host, el_v);
	} else if(!strcmp(el_n,"getthreads/thread/server/socket_port")){
		if (gts->t[gts->l - 1].port) return;
		gts->t[gts->l - 1].port = malloc(strlen(el_v) + 1);
		strcpy(gts->t[gts->l - 1].port, el_v);
	} else if (!strcmp(el_n, "getthreads/thread/label")) {
		if (gts->com) return;
		gts->com = malloc(strlen(el_v) + 1);
		strcpy(gts->com, el_v);
	}
}

static void callback4(void *user, char *el_n, char *at_n, char *at_v) {
	if (strcmp(el_n, "getthreads/thread") || strcmp(at_n, "id")) return;
	struct getthreads *gts = user;
	if (gts->t[gts->l - 1].tid) return;
	gts->t[gts->l - 1].tid = malloc(strlen(at_v) + 1);
	strcpy(gts->t[gts->l - 1].tid, at_v);
}

void freegetthreads(struct getthreads* gts) {
	int i;
	for (i = 0; i < gts->l; i++) {
		if (gts->t[i].host) free(gts->t[i].host);
		if (gts->t[i].port) free(gts->t[i].port);
		if (gts->t[i].tid) free(gts->t[i].tid);
	}
	if (gts->com) free(gts->com);
	free(gts->t);
	free(gts);
}

struct getthreads* getthreads(char* liveid){
  int sock = create_socket("live.nicovideo.jp", "http", 0);
  send(sock, "GET /api/getthreads?v=lv", 24, 0);
  send(sock, liveid, strlen(liveid), 0);
  send(sock, " HTTP/1.1\r\n"
      "Host: live.nicovideo.jp\r\nCookie: user_session=", 57, 0);
  char *session = getSession();
  send(sock, session, strlen(session), 0);
  send(sock, "\r\n\r\n", 4, 0);
  struct getthreads* gts = malloc(sizeof(struct getthreads));
  memset(gts, 0, sizeof(struct getthreads));
  gts->l = 1;
  gts->t = malloc(sizeof(struct ms_thread));
  memset(gts->t, 0, sizeof(struct ms_thread));
  struct xml data;
  memset(&data, 0, sizeof(struct xml));
  data.tag = callback3;
  data.attr = callback4;
  data.user = gts;
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
  gts->t = realloc(gts->t, --gts->l * sizeof(struct ms_thread));
  return gts;
}

int main(int argc, char **argv) {
	WS2I
	struct getthreads *gts = getthreads(argv[1]);
	int i;
	for (i = 0; i < gts->l; i++) {
		printf("%s, %s, %s\n", gts->t[i].host, gts->t[i].port, gts->t[i].tid);
	}
	printf("%s\n", gts->com);
    freegetthreads(gts);
	WS2U
	return 0;
}
