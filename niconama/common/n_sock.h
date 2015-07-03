#if _WIN32
#include <winsock2.h>
#define WS2I WSADATA wsad; WSAStartup(WINSOCK_VERSION, &wsad);
#define WS2U WSACleanup();
#define CLOSESOCKET closesocket
#else
#include <sys/socket.h>
#define WS2I
#define WS2U
#define CLOSESOCKET close
#endif

int create_socket(char*, char*, int);
