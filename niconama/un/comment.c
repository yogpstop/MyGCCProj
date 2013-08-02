#include <pthread.h>
#include "main.h"
#include "xml.h"
#include "gui.h"

char *user_session;
static char userid[64];
static int num, premium;
static char addr[128];
static char port[128];
static char tid[128];
static long cmtime;
static long start_time;

static GtkTreeIter iter;
static GtkListStore*store;

void callback4(struct xml *data) {
	if(!strcmp(data->at_n,"user_id")){
		strcpy(userid,data->at_v);
	}else if(!strcmp(data->at_n,"no")){
		num = atoi(data->at_v);
	}else if(!strcmp(data->at_n,"date")){
		cmtime = atol(data->at_v);
	}else if(!strcmp(data->at_n,"premium")){
		premium = atoi(data->at_v);
	}
}

void callback5(struct xml *data) {
	if(strcmp(data->el_n,".chat"))
		return;
	/*switch(premium){
	case 2:
		fputs("\x1b[31m",stdout);
		break;
	case 3:
		fputs("\x1b[32m",stdout);
		break;
	case 6:
	case 7:
		fputs("\x1b[34m",stdout);
		break;
	}*/
	cmtime=cmtime-start_time;
	char time[32];
	sprintf(time,"%02d:%02d:%02d",(int)(cmtime/60/60),(int)((cmtime/60)%60),(int)(cmtime%60));
	gtk_list_store_prepend(store,&iter);
	gtk_list_store_set(store,&iter,COLUMN_NUM,num,COLUMN_TIME,time,COLUMN_NAME,userid,COLUMN_CHAT,data->el_v,-1);
	if(!premium==2&&!strcmp(data->el_v,"/disconnect"));
		//TODO
}

void printcomment() {
  pthread_t tdd;
  pthread_create(&tdd, NULL, &guimain, &store);
  int sock;
  {
    struct addrinfo *res;
    getaddrinfo(addr, NULL, NULL, &res);
    char thread[100] = "<thread thread=\"";
    strcat(thread, tid);
    strcat(thread, "\" res_from=\"-1\" version=\"20061206\"/>");
    if(res->ai_family == AF_INET6) {
      ((struct sockaddr_in6 *) res->ai_addr)->sin6_port = htons((uint16_t) atoi(port));
    } else {
      ((struct sockaddr_in *) res->ai_addr)->sin_port = htons((uint16_t) atoi(port));
    }
    sock = socket(res->ai_family, SOCK_STREAM, 0);
    connect(sock, res->ai_addr, res->ai_family == AF_INET6 ? v6size : v4size);
    send(sock, thread, strlen(thread) + 1, 0);
    freeaddrinfo(res);
  }
  
  uint8_t recvdata;
  ssize_t recvlen;
  
  struct xml data;
  memset(&data,0,sizeof(struct xml));
  data.attr=callback4;
  data.tag=callback5;
  
  memset(userid,0,64);
  while(1) {
    recvlen = recv(sock, &recvdata, 1, 0);
    if(recvlen < 1)
      break;
    if(recvdata != '\0')
      next(recvdata,&data);
  }

  close(sock);
}

void callback3(struct xml *data) {
	if(!strcmp(data->el_n,".getplayerstatus.ms.addr")){
		strcpy(addr,data->el_v);
	} else if(!strcmp(data->el_n,".getplayerstatus.ms.port")) {
		strcpy(port,data->el_v);
	} else if(!strcmp(data->el_n,".getplayerstatus.ms.thread")) {
		strcpy(tid,data->el_v);
	} else if(!strcmp(data->el_n,".getplayerstatus.stream.start_time")) {
		start_time=atol(data->el_v);
	}
}

void *getliveinfo(void* liveid){
  printf("Start print comment in lv%s\n", (char*)liveid);
  int sock;
  {
    struct addrinfo *res;
    static char thread[200] = "GET /api/getplayerstatus?v=lv";
    strcat(thread, (char*)liveid);
    strcat(thread, " HTTP/1.1\r\nHost: live.nicovideo.jp\r\nCookie: user_session=");
    strcat(thread, user_session);
    strcat(thread, "\r\n\r\n");
    getaddrinfo("live.nicovideo.jp", "http", NULL, &res);
    sock = socket(res->ai_family, SOCK_STREAM, 0);
    connect(sock, res->ai_addr, res->ai_family == AF_INET6 ? v6size : v4size);
    send(sock, thread, strlen(thread), 0);
    freeaddrinfo(res);
  }
  
  uint8_t recvdata;
  ssize_t recvlen;
  
  struct xml data;
  memset(&data,0,sizeof(struct xml));
  data.tag=callback3;

  memset(addr,0,128);
  memset(port,0,128);
  memset(tid,0,128);
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
  printcomment();
  free(liveid);
  return NULL;
}