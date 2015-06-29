#include "main.h"
#include "xml.h"

#define USERID "12612571"


static void callback(void *u, char *el_n, char *el_v) {
	char userid[16],communityid[16];
	char *ptr,*ptr2,*liveid;
	liveid=malloc(16);
	memset(userid,0,16);
	memset(communityid,0,16);
	memset(liveid,0,16);
	ptr=el_v;
	if(ptr!=NULL) {
		ptr2=strchr(el_v,',');
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
					printf("%s,%s,%s\n",liveid,communityid,userid);
				}
			}
		}
	}
}

static void processalert(char *addr, char *port, char *tid) {
  int sock = create_socket(addr, port, SOCK_STREAM);
  send(sock, "<thread thread=\"", 16, 0);
  send(sock, tid, strlen(tid), 0);
  send(sock, "\" res_from=\"-1\" version=\"20061206\" />", 38, 0);
  char recvdata;
  struct xml data;
  memset(&data,0,sizeof(struct xml));
  data.tag=callback;
  while(1) {
    if(1 > recv(sock, &recvdata, 1, 0))
      break;
    if(recvdata != '\0')
      xml_next(recvdata,&data);
  }
  CLOSESOCKET(sock);
}

static void callback2(void *u, char *el_n, char*el_v) {
	if(!strcmp(el_n,"getalertstatus/ms/addr")){
		strcpy(((char**)u)[0],el_v);
	} else if(!strcmp(el_n,"getalertstatus/ms/port")) {
		strcpy(((char**)u)[1],el_v);
	} else if(!strcmp(el_n,"getalertstatus/ms/thread")) {
		strcpy(((char**)u)[2],el_v);
	}
}

void getalertdata(){
  int sock = create_socket("live.nicovideo.jp", "http", 0);
  send(sock, "GET /api/getalertinfo HTTP/1.1\r\n"
      "Host: live.nicovideo.jp\r\n\r\n", 59, 0);
  char recvdata;
  char **user = malloc(3 * sizeof(char*));
  user[0] = malloc(128 * sizeof(char));
  user[1] = malloc(128 * sizeof(char));
  user[2] = malloc(128 * sizeof(char));
  memset(user[0],0,128);
  memset(user[1],0,128);
  memset(user[2],0,128);
  int http = 0;
  struct xml data;
  memset(&data, 0, sizeof(struct xml));
  data.tag = callback2;
  data.user = user;
  while(1) {
    if(1 > recv(sock, &recvdata, 1, 0))
      break;
    if(1 < http)
      xml_next(recvdata,&data);
    else if(recvdata == '\n')
      http++;
    else if(recvdata != '\r')
      http = 0;
  }
  CLOSESOCKET(sock);
  processalert(user[0], user[1], user[2]);
}

int main() {
	WS2I
	getalertdata();
	WS2U
	return 0;
}
