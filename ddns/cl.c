#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

extern int make_sock(const char *, const char *);

int main() {
	char post[] = "d=...&p=...&h=...&i=";
	int sock, http = 0;
	char data[4096], c; 
	sprintf(data, "POST /ddns_req.cgi HTTP/1.0\r\nHost: mc.yogpc.com\r\nConnection: close\r\nContent-Length: %ld\r\n\r\n", strlen(post));
	sock = make_sock("mc.yogpc.com", "http");
	if(sock == -1) return -1;
	send(sock, data, strlen(data), 0);
	send(sock, post, strlen(post), 0);
	memset(data, 0, 4096);
	while(recv(sock, &c, 1, 0) > 0) {
		if(http > 1)
			data[strlen(data)]=c;
		else if(c == '\n')
			http++;
		else if(c != '\r')
			http = 0;
	}
	close(sock);
	fputs(data, stdout);
	return 0;
}
