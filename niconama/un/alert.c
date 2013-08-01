#include <pthread.h>
#include "main.h"
#include "xml.h"

extern void *getliveinfo(void*);

#define USERID "12612571"

static pthread_t tdd;
static char addr[128];
static char port[128];
static char tid[128];

void callback(struct xml *data) {
	char userid[16],communityid[16];
	char *ptr,*ptr2,*liveid;
	liveid=malloc(16);
	memset(userid,0,16);
	memset(communityid,0,16);
	memset(liveid,0,16);
	ptr=data->el_v;
	if(ptr!=NULL) {
		ptr2=strchr(data->el_v,',');
		if(ptr2!=NULL&&(ptr2-ptr)<17) {
			memcpy(liveid,ptr,ptr2-ptr);
			ptr=ptr2;
			ptr2=strchr(ptr+1,',');
			if(ptr2!=NULL&&(ptr2-ptr)<17) {
				memcpy(communityid,ptr+1,ptr2-ptr-1);
				ptr=ptr2;
				ptr2=strchr(ptr+1,0);
				if(ptr2!=NULL&&(ptr2-ptr)<17) {
					memcpy(userid,ptr+1,ptr2-ptr-1);
					if(!strcmp(userid,USERID))
						pthread_create(&tdd, NULL, &getliveinfo, liveid);
					printf("User:%15s   Com:%15s   Live:%15s\n",userid,communityid,liveid);
				}
			}
		}
	}
}

void processalert() {
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
  data.tag=callback;

  while(1) {
    recvlen = recv(sock, &recvdata, 1, 0);
    if(recvlen < 1)
      break;
    if(recvdata != '\0')
      next(recvdata,&data);
  }

  close(sock);
}

void callback2(struct xml *data) {
	if(!strcmp(data->el_n,".getalertstatus.ms.addr")){
		strcpy(addr,data->el_v);
	} else if(!strcmp(data->el_n,".getalertstatus.ms.port")) {
		strcpy(port,data->el_v);
	} else if(!strcmp(data->el_n,".getalertstatus.ms.thread")) {
		strcpy(tid,data->el_v);
	}
}

void getalertdata(){
  int sock;
  {
    struct addrinfo *res;
    static const char thread[] = "GET /api/getalertinfo HTTP/1.1\r\nHost: live.nicovideo.jp\r\n\r\n";
    getaddrinfo("live.nicovideo.jp", "http", NULL, &res);
    sock = socket(res->ai_family, SOCK_STREAM, 0);
    connect(sock, res->ai_addr, res->ai_family == AF_INET6 ? v6size : v4size);
    send(sock, thread, strlen(thread), 0);
    freeaddrinfo(res);
  }

  uint8_t recvdata;
  ssize_t recvlen;  

  memset(addr,0,128);
  memset(port,0,128);
  memset(tid,0,128);
  int http=0;
  struct xml data;
  memset(&data,0,sizeof(struct xml));
  data.tag=callback2;

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
  processalert();
}