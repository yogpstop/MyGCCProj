#include "main.h"

int create_socket(char *host, char *prot, int stype) {
	int sock = -1;
	struct addrinfo *ais, *aip;
	int ret;
	if (!(ret = getaddrinfo(host, prot, NULL, &ais))) {
		for (aip = ais; aip; aip = aip->ai_next) {
			if (stype) aip->ai_socktype = stype;
			sock = socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
			if (-1 == sock) continue;
			if (-1 != connect(sock, aip->ai_addr, aip->ai_addrlen)) break;
			close(sock);
		}
		freeaddrinfo(ais);
	}
	return sock;
}
