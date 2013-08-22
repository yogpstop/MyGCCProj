#include <pthread.h>
#include <time.h>
#include "main.h"
#include "xml.h"
#include "gui.h"
#include "xml_struct.h"

#define C_STR(var); if(!((struct chat_gps*)data->user)->chat.var) { \
			((struct chat_gps*)data->user)->chat.var = malloc(strlen(data->at_v)+1); \
			if(((struct chat_gps*)data->user)->chat.var) \
				strcpy(((struct chat_gps*)data->user)->chat.var,data->at_v); \
		}
#define C_INT(var); ((struct chat_gps*)data->user)->chat.var = atoi(data->at_v);

#define C_LNG(var); ((struct chat_gps*)data->user)->chat.var = atol(data->at_v);
		
#define IF_C_FREE(var); if(((struct chat_gps*)data->user)->chat.var) { \
			free(((struct chat_gps*)data->user)->chat.var); \
		}

struct chat {
	time_t date;
	char* command;
	int num;
	int premium;
	char* userid;
	int score;
};

struct chat_gps {
	struct getplayerstatus* gps;
	struct chat chat;
};

static GtkTreeIter iter;
static GtkListStore*store;

static void callback4(struct xml *data) {
	if(!strcmp(data->at_n,"user_id")) {
		C_STR(userid);
	} else if(!strcmp(data->at_n,"mail")) {
		C_STR(command);
	} else if(!strcmp(data->at_n,"no")) {
		C_INT(num);
	} else if(!strcmp(data->at_n,"premium")) {
		C_INT(premium);
	} else if(!strcmp(data->at_n,"score")) {
		C_INT(score);
	} else if(!strcmp(data->at_n,"date")) {
		C_LNG(date);
	}
}

static void callback5(struct xml *data) {
	if(!strcmp(data->el_n,"chat")) {
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
		((struct chat_gps*)data->user)->chat.date=((struct chat_gps*)data->user)->chat.date-((struct chat_gps*)data->user)->gps->start_time;
		char time[32];
		sprintf(time,"%02d:%02d:%02d",(int)(((struct chat_gps*)data->user)->chat.date/60/60),(int)((((struct chat_gps*)data->user)->chat.date/60)%60),(int)(((struct chat_gps*)data->user)->chat.date%60));
		gtk_list_store_prepend(store,&iter);
		gtk_list_store_set(store,&iter,COLUMN_NUM,((struct chat_gps*)data->user)->chat.num,COLUMN_TIME,time,COLUMN_NAME,((struct chat_gps*)data->user)->chat.userid,COLUMN_CHAT,data->el_v,-1);
		if(!((struct chat_gps*)data->user)->chat.premium==2&&!strcmp(data->el_v,"/disconnect"));
			//TODO
	}
	IF_C_FREE(userid);
	IF_C_FREE(command);
	memset(&((struct chat_gps*)data->user)->chat,0,sizeof(struct chat));
}

void printcomment(struct getplayerstatus* gps) {
  pthread_t tdd;
  pthread_create(&tdd, NULL, &guimain, &store);
  int sock;
  {
    struct addrinfo *res;
    getaddrinfo(gps->ms_addr, NULL, NULL, &res);
    char thread[100];
    sprintf(thread,"<thread thread=\"%ld\" res_from=\"-1\" version=\"20061206\" scores=\"1\" />",gps->ms_thread);
    if(res->ai_family == AF_INET6) {
      ((struct sockaddr_in6 *) res->ai_addr)->sin6_port = htons(gps->ms_port);
    } else {
      ((struct sockaddr_in *) res->ai_addr)->sin_port = htons(gps->ms_port);
    }
    sock = socket(res->ai_family, SOCK_STREAM, 0);
    connect(sock, res->ai_addr, res->ai_addrlen);
    send(sock, thread, strlen(thread) + 1, 0);
    freeaddrinfo(res);
  }
  
  uint8_t recvdata;
  ssize_t recvlen;
  
  struct xml data;
  struct chat_gps cdata;
  memset(&data,0,sizeof(struct xml));
  memset(&cdata,0,sizeof(struct chat_gps));
  cdata.gps=gps;
  data.attr=callback4;
  data.tag=callback5;
  data.user=&cdata;
  
  while(1) {
    recvlen = recv(sock, &recvdata, 1, 0);
    if(recvlen < 1)
      break;
    if(recvdata != '\0')
      next(recvdata,&data);
  }
  close(sock);
}