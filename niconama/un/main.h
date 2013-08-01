// C Standard Library
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Unix Standard Library
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define PORT 0x1
#define MS 0x2
#define ADDR 0x4
#define THREAD 0x8
#define TAG 0x10
#define DTAG 0x20
#define ATTR 0x40
#define BASE_TIME 0x80
#define START_TIME 0x100
#define v6size sizeof(struct sockaddr_in6)
#define v4size sizeof(struct sockaddr_in)