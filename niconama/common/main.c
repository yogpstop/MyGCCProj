#if _WIN32
#include <winsock2.h>
#endif
#include "cookie.h"
#include "data_exchange.h"
#include "rtmp.h"
#include "coreaudio.h"
#include "fdk-aac.h"
#include <stdio.h>

void createMainGUI();

int main() {
	fputs("Enter main\n",stdout);
	getSession();
#if _WIN32
	WSADATA wsad;
	WSAStartup(WINSOCK_VERSION,&wsad);
	CoInitializeEx(NULL, 0);
#endif
	fputs("Done initialize\n",stdout);
	createMainGUI();
#if _WIN32
	CoUninitialize();
#endif
	return 0;
}

THREAD_RET_TYPE start_streaming_thread(void *dummy) {
	if(rtmp_init()) return THREAD_RETURN_SUCCESS;
	if(fdk_aac_init()) return THREAD_RETURN_SUCCESS;
	if(windows_core_audio_init()) return THREAD_RETURN_SUCCESS;
	if(rtmp_write_header(NULL, fdk_aac_header)) return THREAD_RETURN_SUCCESS;
	if(windows_core_audio_sync()) return THREAD_RETURN_SUCCESS;
	THREAD_CREATE_WIN2(windows_core_audio_loop, NULL);
	THREAD_CREATE_WIN2(fdk_aac_loop, NULL);
	THREAD_CREATE_WIN2(rtmp_loop, NULL);
	return THREAD_RETURN_SUCCESS;
}
