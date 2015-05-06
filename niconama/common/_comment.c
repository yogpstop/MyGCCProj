#include "main.h"
#include "xml.h"
#include "xml_struct.h"

static void callback4(void *user, char *el_n, char *at_n, char *at_v) {
	if (strcmp("chat", el_n)) return;
	struct chat *c = user;
	if (!strcmp(at_n, "no")) c->no = strtoul(at_v, NULL, 10);
	else if (!strcmp(at_n, "vpos")) c->vpos = strtoll(at_v, NULL, 10);
	else if (!strcmp(at_n, "date")) c->date = strtoull(at_v, NULL, 10);
	else if (!strcmp(at_n, "date_usec"))
		c->date_usec = strtoull(at_v, NULL, 10);
	else if (!strcmp(at_n, "anonymity")) c->anonymity = strtol(at_v, NULL, 10);
	else if (!strcmp(at_n, "premium")) c->premium = strtol(at_v, NULL, 10);
	else if (!strcmp(at_n, "scores")) c->scores = strtol(at_v, NULL, 10);
	else if (!strcmp(at_n, "mail")) {
		if (c->mail) free(c->mail); c->mail = malloc(strlen(at_v) + 1);
		if (c->mail) strcpy(c->mail, at_v);
	} else if (!strcmp(at_n, "user_id")) {
		if (c->user_id) free(c->user_id); c->user_id = malloc(strlen(at_v) + 1);
		if (c->user_id) strcpy(c->user_id, at_v);
	} else if (!strcmp(at_n, "name")) {
		if (c->name) free(c->name); c->name = malloc(strlen(at_v) + 1);
		if (c->name) strcpy(c->name, at_v);
	}
}

static void callback5(void *user, char *el_n, char *el_v) {
	if (strcmp("chat", el_n)) return;
	struct chat *c = user;
	if (c->premium == 7) fputs("\e[34m", stdout);
	else if (c->premium == 6) fputs("\e[31m", stdout);
	else if (c->premium == 3) fputs("\e[32m", stdout);
	//TODO no vpos(date) scores user_id(name,fhn)
	fputs(el_v, stdout);
	fputs("\n\e[0m", stdout);
	fflush(stdout);
	if (c->mail) free(c->mail);
	if (c->user_id) free(c->user_id);
	if (c->name) free(c->name);
	memset(c, 0, sizeof(struct chat));
}

void getcomment(char *addr, char *port, char *tid) {
  int sock = create_socket(addr, port, SOCK_STREAM);
  send(sock, "<thread thread=\"", 16, 0);
  send(sock, tid, strlen(tid), 0);
  send(sock, "\" res_from=\"-1\" version=\"20061206\" scores=\"1\" />", 49, 0);
  char recvdata;
  struct xml data;
  struct chat c;
  memset(&c, 0, sizeof(struct chat));
  memset(&data, 0, sizeof(struct xml));
  data.attr=callback4;
  data.tag=callback5;
  data.user = &c;
  while(1) {
    if (1 > recv(sock, &recvdata, 1, 0))
      break;
    if (recvdata != '\0')
      xml_next(recvdata, &data);
  }
  close(sock);
}

int main(int argc, char **argv) {
	if (argc != 2) return 1;
	WS2I
	struct getplayerstatus *gps = getplayerstatus(argv[1]);
	getcomment(gps->ms_addr, gps->ms_port, gps->ms_thread);
	freegetplayerstatus(gps);
	WS2U
	return 0;
}
