//!/usr/bin/gcc -Wall -Werror -O3 -s -o wol wol.c
//!/usr/bin/gcc -Wall -Werror -O3 -s -o wol.exe wol.c -lws2_32
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif
#include <string.h>

#define M1 0x68
#define M2 0x05
#define M3 0xCA
#define M4 0x05
#define M5 0x8E
#define M6 0x05

#define IP1 192
#define IP2 168
#define IP3 193

static const unsigned char tosend[6+6*16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			M1, M2, M3, M4, M5, M6, M1, M2, M3, M4, M5, M6,
			M1, M2, M3, M4, M5, M6, M1, M2, M3, M4, M5, M6,
			M1, M2, M3, M4, M5, M6, M1, M2, M3, M4, M5, M6,
			M1, M2, M3, M4, M5, M6, M1, M2, M3, M4, M5, M6,
			M1, M2, M3, M4, M5, M6, M1, M2, M3, M4, M5, M6,
			M1, M2, M3, M4, M5, M6, M1, M2, M3, M4, M5, M6,
			M1, M2, M3, M4, M5, M6, M1, M2, M3, M4, M5, M6,
			M1, M2, M3, M4, M5, M6, M1, M2, M3, M4, M5, M6};
static const int brdcst = 1;
#ifndef _WIN32
static const struct sockaddr_in cli = {AF_INET, 0, {INADDR_ANY}},
	srv = {AF_INET, 0x0900, {IP1 | IP2 << 8 | IP3 << 16 | 255 << 24}};
#else
static const struct sockaddr_in cli = {AF_INET, 0, {{.S_addr = INADDR_ANY}}},
	srv = {AF_INET, 0x0900, {{.S_addr = IP1 | IP2 << 8 | IP3 << 16 | 255 << 24}}};
#endif
int main() {
	const int sock = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void*)&brdcst, sizeof(brdcst));
	bind(sock, (struct sockaddr *)&cli, sizeof(cli));
	sendto(sock, (void*)&tosend, 6+6*16, 0, (struct sockaddr *)&srv, sizeof(srv));
	return 0;
}
