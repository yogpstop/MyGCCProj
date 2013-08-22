#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int make_sock(const char *host, const char *service) {
	int sock;
	struct addrinfo *res0, *res;
	if(0 != getaddrinfo(host, service, NULL, &res0)) return -1;
	for(res = res0; res != NULL; res = res->ai_next) {
		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(-1 == sock) continue;
		if(-1 == connect(sock, res->ai_addr, res->ai_addrlen)) {
			close(sock);
			continue;
		}
		break;
	}
	freeaddrinfo(res0);
	if(res == NULL) return -1;
	return sock;
}

