#if _WIN32
#include <winsock2.h>
#endif
#include "cookie.h"
#include "data_exchange.h"
#include "rtmp.h"
#include "coreaudio.h"
#include "fdk-aac.h"

void createMainGUI();

int main() {
	getSession();
#if _WIN32
	WSADATA wsad;
	WSAStartup(WINSOCK_VERSION,&wsad);
	CoInitialize(NULL);
#endif
	createMainGUI();
#if _WIN32
	CoUninitialize();
#endif
	return 0;
}

THREAD_RET_TYPE start_streaming_thread(void *dummy) {
	if(rtmp_init()) return -1;
	if(fdk_aac_init()) return -2;
	if(windows_core_audio_init()) return -3;
	if(rtmp_write_header(NULL, fdk_aac_header)) return -4;
	if(windows_core_audio_sync()) return -5;
	CreateThread(NULL, 0, windows_core_audio_loop, NULL, 0, NULL);
	CreateThread(NULL, 0, fdk_aac_loop, NULL, 0, NULL);
	CreateThread(NULL, 0, rtmp_loop, NULL, 0, NULL);
	//TODO create thread
	return 0;
}
