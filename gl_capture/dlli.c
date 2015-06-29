#include <stdio.h>
#include <windows.h>
#include <stdint.h>
#include "vfw.h"
#include "main.h"
#include "dbg.h"

static void dll_inject(char *dll, int pid) {
	HANDLE proc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE
			| PROCESS_CREATE_THREAD, FALSE, pid);
	void *lla = GetProcAddress(GetModuleHandleA("kernel32.dll"),
			"LoadLibraryA");
	void *vm = VirtualAllocEx(proc, NULL, strlen(dll) + 1,
			MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(proc, vm, dll, strlen(dll) + 1, NULL);
	HANDLE h = CreateRemoteThread(proc, NULL, 0, lla, vm, 0, NULL);
	WaitForSingleObject(h, INFINITE);
	CloseHandle(h);
	VirtualFreeEx(proc, vm, 0, MEM_RELEASE);
	//FIXME GetModuleHandleA GetProcAddress (free?)
	CloseHandle(proc);
}

int main(int argc, char **argv) {
	if (argc != 3) return 1;
	DWORD pid = strtoul(argv[2], NULL, 10);
	dll_inject(argv[1], pid);
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
