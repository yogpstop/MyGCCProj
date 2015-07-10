#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "n_sock.h"
#include "cookie.h"
#define CACHE_DIR "V:\\nv\\"

static void sep_dec_url(char *src, char **host, char **url) {
	char *dst = src; int http = 0;
	while (*src) {
		if (*src != '%') *dst++ = *src++;
		else {
			if (src[1] <= '9') src[1] -= '0';
			else { src[1] &= 0x5F; src[1] -= 'A' - 10; }
			if (src[2] <= '9') src[2] -= '0';
			else { src[2] &= 0x5F; src[2] -= 'A' - 10; }
			*dst++ = (src[1] << 4) | src[2]; src += 3;
		}
		if (http == 0 && dst[-1] == ':') http = 1;
		else if (http == 1 && dst[-1] == '/') http = 2;
		else if (http == 2 && dst[-1] == '/')
				{ http = 3; dst[-3] = 0; *host = --dst; }
		else if (http == 3 && dst[-1] == '/')
				{ http = 4; dst[-1] = 0; *(*url = dst++) = '/'; }
		else if (http < 3) http = 0;
	}
	*dst = 0;
}

void nv_get(const char *vid) {
	WS2I
	const char * const us = getSession();
	char * const buf = malloc(1024 * 1024);//1MB cache
	char *ptr, *host, *url;
	int sock = create_socket("www.nicovideo.jp", "http", 0), len;
	send(sock, "HEAD /watch/", 12, 0);
	send(sock, vid, strlen(vid), 0);
	send(sock, " HTTP/1.0\r\nHost: www.nicovideo.jp\r\nCookie: user_session=", 56, 0);
	send(sock, us, strlen(us), 0);
	send(sock, "\r\n\r\n", 4, 0);
	buf[recv(sock, buf, 1024 * 1024, MSG_WAITALL)] = 0;
	CLOSESOCKET(sock);
	ptr = strstr(buf, "nicohistory=") + 12; *strchr(ptr, ';') = 0;
	strcpy(buf + 1024 * 1023, ptr);
	// ---------------------------------------------------------------------------------
	sock = create_socket("flapi.nicovideo.jp", "http", 0);
	send(sock, "GET /api/getflv?v=", 18, 0);
	send(sock, vid, strlen(vid), 0);
	send(sock, " HTTP/1.0\r\nHost: flapi.nicovideo.jp\r\nCookie: user_session=", 58, 0);
	send(sock, us, strlen(us), 0);
	send(sock, "\r\n\r\n", 4, 0);
	buf[recv(sock, buf, 1024 * 1023, MSG_WAITALL)] = 0;
	CLOSESOCKET(sock);
	ptr = strstr(buf, "url=") + 4; *strchr(ptr, '&') = 0;
	sep_dec_url(ptr, &host, &url);
	// ---------------------------------------------------------------------------------
	sock = create_socket(host, ptr, 0);
	send(sock, "GET ", 4, 0);
	send(sock, url, strlen(url), 0);
	send(sock, " HTTP/1.0\r\nHost: ", 17, 0);
	send(sock, host, strlen(host), 0);
	send(sock, "\r\nCookie: nicohistory=", 22, 0);
	send(sock, buf + 1024 * 1023, strlen(buf + 1024 * 1023), 0);
	send(sock, "; user_session=", 15, 0);
	send(sock, us, strlen(us), 0);
	send(sock, "\r\n\r\n", 4, 0);
	memcpy(buf, CACHE_DIR, 6);
	strcpy(buf + 6, vid);
	FILE *f = fopen(buf, "wb");
	int http = 0;
	while ((len = recv(sock, buf, 1024 * 1024, MSG_WAITALL))) {
		ptr = buf; url = ptr + len;
		if (http >= 2) goto L2;
L1:		if (*ptr == '\n') { if (++http >= 2) { ptr++; goto L2; }
		} else if (*ptr != '\r') http = 0;
		if (++ptr < url) goto L1;
		continue;
L2:		fwrite(ptr, 1, url - ptr, f);
	}
	CLOSESOCKET(sock);
	fclose(f);
	WS2U
}
int main(int argc, char **argv) {
	if (argc != 2) return 1;
	nv_get(argv[1]);
	return 0;
}
