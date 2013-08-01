#include "main.h"

char *user_session;

void printcomment(char* server, char* port, char* tid, const long offset) {
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
  char userid[64];
  int num=0,premium=0;
  long vpos=0;
  char *ptr,*ptr2,*ptrt;
  memset(buf,0,512);
  memset(userid,0,64);
  while(1) {
    recvlen = recv(sock, &recvdata, 1, 0);
    if(recvlen < 1)
      break;
    if(recvdata == '\0') {
      ptrt = strchr(buf,'>');
      if(ptrt!=NULL) {
        ptr = strstr(buf,"user_id");
        if(ptr!=NULL&&ptr<ptrt) {
          ptr = strchr(ptr,'"');
          if(ptr!=NULL) {
            ptr2 = strchr(ptr+1,'"');
            if(ptr2!=NULL&&(ptr2-ptr)<65) {
              memcpy(userid,ptr+1,ptr2-ptr-1);
            }
          }
        }
        ptr = strstr(buf,"no");
        if(ptr!=NULL&&ptr<ptrt) {
          ptr = strchr(ptr,'"');
          if(ptr!=NULL) {
            ptr2 = strchr(ptr+1,'"');
            if(ptr2!=NULL) {
              char cache[ptr2-ptr];
              memset(cache,0,ptr2-ptr);
              memcpy(cache,ptr+1,ptr2-ptr-1);
              num=atoi(cache);
            }
          }
        }
        ptr = strstr(buf,"vpos");
        if(ptr!=NULL&&ptr<ptrt) {
          ptr = strchr(ptr,'"');
          if(ptr!=NULL) {
            ptr2 = strchr(ptr+1,'"');
            if(ptr2!=NULL) {
              char cache[ptr2-ptr];
              memset(cache,0,ptr2-ptr);
              memcpy(cache,ptr+1,ptr2-ptr-1);
              vpos=atol(cache);
            }
          }
        }
        ptr = strstr(buf,"premium");
        if(ptr!=NULL&&ptr<ptrt) {
          ptr = strchr(ptr,'"');
          if(ptr!=NULL) {
            ptr2 = strchr(ptr+1,'"');
            if(ptr2!=NULL) {
              char cache[ptr2-ptr];
              memset(cache,0,ptr2-ptr);
              memcpy(cache,ptr+1,ptr2-ptr-1);
              premium=atoi(cache);
            }
          }
        }
        ptr2 = strchr(ptrt+1,'<');
        if(ptr2!=NULL) {
          *ptr2=0;
          switch(premium){
          case 2:
            puts("\x1b[31m");
            break;
          case 3:
            puts("\x1b[32m");
            break;
          case 6:
          case 7:
            puts("\x1b[34m");
            break;
          }
          vpos-=offset;
          printf("(%02d:%02d:%02d.%02d)[%d]<%s>%s\x1b[39m\n",(int)(vpos/60/60/100),(int)((vpos/60/100)%60),(int)((vpos/100)%60),(int)(vpos%100),num,userid,ptrt+1);
          if(!premium==2&&!strcmp(ptrt+1,"/disconnect"))
            break;
          }
      }
      memset(buf,0,512);
      memset(userid,0,64);
      num=0;
      premium=0;
      vpos=0;
    } else
      buf[strlen(buf)] = recvdata;
  }

  close(sock);
  freeaddrinfo(res);
}

void *getliveinfo(void* liveid){
  printf("Start print comment in lv%s\n",(char*)liveid);
  int sock;
  uint8_t recvdata;
  ssize_t recvlen;
  struct addrinfo *res;
  getaddrinfo("live.nicovideo.jp", "http", NULL, &res);
  
  static char thread[200] = "GET /api/getplayerstatus?v=lv";
  strcat(thread, (char*)liveid);
  strcat(thread, " HTTP/1.1\r\nHost: live.nicovideo.jp\r\nCookie: user_session=");
  strcat(thread, user_session);
  strcat(thread, "\r\n\r\n");
  
  sock = socket(res->ai_family, SOCK_STREAM, 0);

  connect(sock, res->ai_addr, res->ai_family == AF_INET6 ? v6size : v4size);
  
  send(sock, thread, strlen(thread), 0);
  
  int xml=0,element=0;
  char tagname[64];
  memset(tagname,0,64);
  
  char addr[128];
  memset(addr,0,128);
  char port[128];
  memset(port,0,128);
  char tid[128];
  memset(tid,0,128);
  char base[16];
  memset(base,0,16);
  char start[16];
  memset(start,0,16);
  while(1) {
    recvlen = recv(sock, &recvdata, 1, 0);
    if(recvlen < 1)
      break;
    if(xml == 2) {
      if(recvdata=='<') {
        element|=TAG;
      } else if(recvdata=='/'&&(element&TAG)==TAG&&tagname[0]==0) {
        element|=DTAG;
      } else if(recvdata=='>'&&(element&TAG)==TAG) {
        if(!strcmp(tagname,"ms")) {
          if((element&DTAG)==DTAG)
            element&=0xFFF ^ MS;
          else
            element|=MS;
        } else if((element&MS)==MS&&!strcmp(tagname,"addr")) {
          if((element&DTAG)==DTAG)
            element&=0xFFF ^ ADDR;
          else
            element|=ADDR;
        } else if((element&MS)==MS&&!strcmp(tagname,"port")) {
         if((element&DTAG)==DTAG)
            element&=0xFFF ^ PORT;
          else
            element|=PORT;
        } else if((element&MS)==MS&&!strcmp(tagname,"thread")) {
          if((element&DTAG)==DTAG)
            element&=0xFFF ^ THREAD;
          else
            element|=THREAD;
        } else if(!strcmp(tagname,"base_time")) {
          if((element&DTAG)==DTAG)
            element&=0xFFF ^ BASE_TIME;
          else
            element|=BASE_TIME;
        } else if(!strcmp(tagname,"start_time")) {
          if((element&DTAG)==DTAG)
            element&=0xFFF ^ START_TIME;
          else
            element|=START_TIME;
        }
        element&=0xFFF ^ (TAG | DTAG | ATTR);
        memset(tagname,0,64);
      } else if(recvdata==' '&&(element&TAG)==TAG) {
        if(tagname[0]!=0)
          element|=ATTR;
      } else if((element&(TAG|ATTR))==TAG) {
       tagname[strlen(tagname)]=recvdata;
      } else if((element&ADDR)==ADDR) {
        addr[strlen(addr)]=recvdata;
      } else if((element&PORT)==PORT) {
        port[strlen(port)]=recvdata;
      } else if((element&THREAD)==THREAD) {
        tid[strlen(tid)]=recvdata;
      } else if((element&START_TIME)==START_TIME) {
        start[strlen(start)]=recvdata;
      } else if((element&BASE_TIME)==BASE_TIME) {
        base[strlen(base)]=recvdata;
      }
    } else if(recvdata == '\n')
      xml++;
    else if(recvdata != '\r')
      xml = 0;
  }
  close(sock);
  freeaddrinfo(res);
  printcomment(addr,port,tid,(atol(start)-atol(base))*100);
  free(liveid);
  return NULL;
}