#include <stdio.h>
#include <windows.h>
#include <stdint.h>
#include "vfw.h"
#include "main.h"
#include "dbg.h"

int main(int argc, char **argv) {
	if (argc != 2) return 1;
	DWORD pid = strtoul(argv[1], NULL, 10);
	char *path = malloc(MAX_PATH);
	GetModuleFileNameA(NULL, path, MAX_PATH);
	*strrchr(path, '\\') = 0;
	HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	BOOL w32;
#if defined(_WIN64)
	IsWow64Process(proc, &w32);
#elif defined(_WIN32)
	w32 = TRUE;
#endif
	CloseHandle(proc);
	const char *bit = w32 ? "32" : "64";
	char *full = malloc((strlen(path) + 11) * 2 + 17);
	sprintf(full, "\"%s\\hook%s.exe\" \"%s\\hook%s.dll\" %I32u", path, bit, path, bit, pid);
	STARTUPINFO si = {};
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi = {};
	CreateProcess(NULL, full, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	free(full);
	free(path);
	HANDLE mutex[2], event;
	char tmp[32], *cptr;
	sprintf(tmp, "GLC_SM_%I32u_", pid);
	cptr = tmp + strlen(tmp);
	strcpy(cptr, "EVENT");
	event = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, tmp);
	strcpy(cptr, "MUTEX0");
	mutex[0] = OpenMutex(SYNCHRONIZE, FALSE, tmp);
	strcpy(cptr, "MUTEX1");
	mutex[1] = OpenMutex(SYNCHRONIZE, FALSE, tmp);
	strcpy(cptr, "FILE");
	HANDLE fm = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, tmp);
	void *fm_buf = MapViewOfFile(fm, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	vfw_str *vfwctx = vfw_init();
	int i;
	DBG_PERF_INIT(3, 8, GLC_FPS * 60 * 60);
	while (WaitForSingleObject(event, 5000) == WAIT_OBJECT_0) {
		DBG_PERF(3, 0);
		i = *((uint8_t*)fm_buf + GLC_MAX_SIZE * 2);
		DBG_PERF(3, 1);
		WaitForSingleObject(mutex[i], INFINITE);
		DBG_PERF(3, 2);
		ResetEvent(event);
		DBG_PERF(3, 3);
		vfw_frame(vfwctx, fm_buf + GLC_MAX_SIZE * i);
		DBG_PERF(3, 6);
		ReleaseMutex(mutex[i]);
		DBG_PERF(3, 7);
		DBG_PERF_INC(3);
	}
	DBG_PERF_FLUSH();
	vfw_end(vfwctx);
	UnmapViewOfFile(fm_buf);
	CloseHandle(fm);
	CloseHandle(mutex[1]);
	CloseHandle(mutex[0]);
	CloseHandle(event);
	return 0;
}
