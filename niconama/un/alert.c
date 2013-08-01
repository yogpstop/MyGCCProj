#include <pthread.h>
#include "main.h"

extern void *getliveinfo(void*);

#define USERID "12612571"

static pthread_t tdd;

void processalert(char* server, char* port, char* tid) {
  int sock;
  uint8_t recvdata;
  ssize_t recvlen;
  struct addrinfo *res;
  getaddrinfo(server, NULL, NULL, &res);
  
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
  
  char buf[512];
  char communityid[16],userid[16];
  char *ptr,*ptr2,*liveid;
  liveid=malloc(16);
  memset(buf,0,512);
  memset(userid,0,16);
  memset(liveid,0,16);
  memset(communityid,0,16);

  while(1) {
    recvlen = recv(sock, &recvdata, 1, 0);
    if(recvlen < 1)
      break;
    if(recvdata == '\0') {
      ptr=strchr(buf,'>');
      if(ptr!=NULL) {
        ptr2=strchr(ptr+1,',');
        if(ptr2!=NULL&&(ptr2-ptr)<17) {
          memcpy(liveid,ptr+1,ptr2-ptr-1);
          ptr=ptr2;
          ptr2=strchr(ptr+1,',');
          if(ptr2!=NULL&&(ptr2-ptr)<17) {
            memcpy(communityid,ptr+1,ptr2-ptr-1);
            ptr=ptr2;
            ptr2=strchr(ptr+1,'<');
            if(ptr2!=NULL&&(ptr2-ptr)<17) {
              memcpy(userid,ptr+1,ptr2-ptr-1);
              if(!strcmp(userid,USERID)){
                pthread_create(&tdd, NULL, &getliveinfo, liveid);
                liveid=malloc(16);
              }
            }
          }
        }
      }
      memset(buf,0,512);
      memset(userid,0,16);
      memset(liveid,0,16);
      memset(communityid,0,16);
    } else
      buf[strlen(buf)] = recvdata;
  }

  close(sock);
  freeaddrinfo(res);
}

void getalertdata(){
  int sock;
  uint8_t recvdata[128];
  ssize_t recvlen;
  struct addrinfo *res;
  getaddrinfo("live.nicovideo.jp", "http", NULL, &res);
  
  static const char thread[] = "GET /api/getalertinfo HTTP/1.1\r\nHost: live.nicovideo.jp\r\n\r\n";
  
  sock = socket(res->ai_family, SOCK_STREAM, 0);

  connect(sock, res->ai_addr, res->ai_family == AF_INET6 ? v6size : v4size);
  
  send(sock, thread, strlen(thread), 0);
  
  int i,xml=0,element=0;
  char tagname[64];
  memset(tagname,0,64);

  char addr[128];
  memset(addr,0,128);
  char port[128];
  memset(port,0,128);
  char tid[128];
  memset(tid,0,128);

  while(1) {
    recvlen = recv(sock, recvdata, 128, 0);
    if(recvlen < 1)
      break;
    for(i=0;i<recvlen;i++) {
      if(xml == 2) {
        if(recvdata[i]=='<') {
          element|=TAG;
        }else if(recvdata[i]=='/'&&(element&TAG)==TAG&&tagname[0]==0){
          element|=DTAG;
        }else if(recvdata[i]=='>'&&(element&TAG)==TAG){
          if(!strcmp(tagname,"ms")){
            if((element&DTAG)==DTAG)
              element&=0xFF ^ MS;
            else
              element|=MS;
          } else if((element&MS)==MS&&!strcmp(tagname,"addr")) {
          	if((element&DTAG)==DTAG)
              element&=0xFF ^ ADDR;
            else
              element|=ADDR;
          } else if((element&MS)==MS&&!strcmp(tagname,"port")) {
          	if((element&DTAG)==DTAG)
              element&=0xFF ^ PORT;
            else
              element|=PORT;
          } else if((element&MS)==MS&&!strcmp(tagname,"thread")) {
          	if((element&DTAG)==DTAG)
              element&=0xFF ^ THREAD;
            else
              element|=THREAD;
          }
          element&=0xFF ^ (TAG | DTAG | ATTR);
          memset(tagname,0,64);
        }else if(recvdata[i]==' '&&(element&TAG)==TAG){
          if(tagname[0]!=0)
            element|=ATTR;
        }else if((element&(TAG|ATTR))==TAG){
      	  tagname[strlen(tagname)]=recvdata[i];
        }else if((element&ADDR)==ADDR){
          addr[strlen(addr)]=recvdata[i];
        }else if((element&PORT)==PORT){
          port[strlen(port)]=recvdata[i];
        }else if((element&THREAD)==THREAD){
          tid[strlen(tid)]=recvdata[i];
        }
        continue;
      }
      if(recvdata[i] == '\n')
        xml++;
      else if(recvdata[i] != '\r')
        xml = 0;
    }
  }
  close(sock);
  freeaddrinfo(res);
  processalert(addr,port,tid);
}