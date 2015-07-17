#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif

#define _MAC(a1, a2, a3, a4, a5, a6) 0x ## a1, 0x ## a2, 0x ## a3, 0x ## a4, 0x ## a5, 0x ## a6
#define MAC _MAC(C8, 60, 00, 22, 95, 12)

static const char tosend[6+6*16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			MAC, MAC, MAC, MAC, MAC, MAC, MAC, MAC,
			MAC, MAC, MAC, MAC, MAC, MAC, MAC, MAC};
static const int brdcst = 1;
#ifndef _WIN32
static const struct sockaddr_in srv = {AF_INET, 0x0900, {0xFFFFFFFF}};
#else
static const struct sockaddr_in srv = {AF_INET, 0x0900, {{.S_addr = 0xFFFFFFFF}}};
#endif
int main() {
#ifdef _WIN32
	WSADATA wsad; WSAStartup(WINSOCK_VERSION, &wsad);
#endif
	const int sock = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void*)&brdcst, sizeof(brdcst));
	sendto(sock, tosend, 6+6*16, 0, (struct sockaddr *)&srv, sizeof(srv));
#ifndef _WIN32
	close(sock);
#else
	closesocket(sock);
	WSACleanup();
#endif
	return 0;
}
