#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

extern int make_sock(const char *, const char *);

int main(){
	fputs("\r\n\r\n", stdout);
	int sock, len = atoi(getenv("CONTENT_LENGTH")), http = 0;
	char buf[1024] = "GET /cgi-bin/dyn.fcg?";
	read(0, buf + strlen(buf), len);
	strcat(buf, getenv("REMOTE_ADDR"));
	strcat(buf, " HTTP/1.0\r\nHost: dyn.value-domain.com\r\nConnection: close\r\n\r\n");
	sock = make_sock("dyn.value-domain.com", "http");
	if(sock == -1) return -1;
	send(sock, buf, strlen(buf), 0);
	while((len = recv(sock, buf, 1, 0)) > 0) {
		if(http > 1)
			fputc(buf[0], stdout);
		else if(buf[0] == '\n')
			http++;
		else if(buf[0] != '\r')
			http = 0;
	}
	close(sock);
	return 0;
}
