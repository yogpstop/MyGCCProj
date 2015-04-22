#pragma once

// C Standard Library
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Unix Standard Library
#include <unistd.h>
#include <sys/types.h>
#if _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#define PORT 0x1
#define MS 0x2
#define ADDR 0x4
#define THREAD 0x8
#define TAG 0x10
#define DTAG 0x20
#define ATTR 0x40
#define BASE_TIME 0x80
#define START_TIME 0x100

int create_socket(char*, char*, int);
