#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "MinHook.h"
#include "main.h"
#include "dbg.h"
#include "hook.h"

HINSTANCE hInst;
static HANDLE v_t_copy, v_t_init;

static HANDLE mutex[BUF_SIZE], event;
static HANDLE file_map;
void *fm_buf;

LARGE_INTEGER qpc_real, qpc_target;
int interval;

static HANDLE eBreak;
HANDLE eWake;
int gidx;
void *vatex;

CRITICAL_SECTION lock[BUF_SIZE];
LARGE_INTEGER qpc[BUF_SIZE];
RECT rect[BUF_SIZE];

static DWORD WINAPI copy_thread(LPVOID lpp) {
	int idx; void *inp, *outp;
	while (WaitForSingleObject(eWake, INFINITE) == WAIT_OBJECT_0) {
		DBG_PERF(2, 0);
		if (WaitForSingleObject(eBreak, 0) == WAIT_OBJECT_0) { ResetEvent(eWake); break; }
		idx = gidx;
		inp = vatex;
		DBG_PERF(2, 1);
		EnterCriticalSection(lock + idx);
		DBG_PERF(2, 2);
		ResetEvent(eWake);
		outp = fm_buf + idx * GLC_MAX_SIZE;
		if (WaitForSingleObject(event, 0) == WAIT_TIMEOUT) {
			DBG_PERF(2, 3);
			if (WaitForSingleObject(mutex[idx], 0) == WAIT_OBJECT_0) {
				*((LONGLONG*)outp) = qpc[idx].QuadPart;
				*((uint16_t*)(outp + sizeof(LONGLONG))) = rect[idx].right;
				*((uint16_t*)(outp + sizeof(LONGLONG) + sizeof(uint16_t))) = rect[idx].bottom;
				DBG_PERF(2, 4);
				memcpy(outp + GLC_HDR_SIZE, inp, rect[idx].right * rect[idx].bottom * GLC_COLORDEPTH);
				DBG_PERF(2, 5);
				ReleaseMutex(mutex[idx]);
				*((uint8_t*)fm_buf + GLC_MAX_SIZE * BUF_SIZE) = idx;
				SetEvent(event);
			}
		}
		LeaveCriticalSection(lock + idx);
		DBG_PERF(2, 6);
		DBG_PERF_INC(2);
	}
	return 0;
}

static DWORD WINAPI init_thread(LPVOID mainThread) {
	WaitForSingleObject(mainThread, INFINITE);
	CloseHandle(mainThread);
	QueryPerformanceFrequency(&qpc_real);
	interval = qpc_real.QuadPart / GLC_FPS;
	QueryPerformanceCounter(&qpc_target);
	DBG_PERF_INIT(2, 7, GLC_FPS * 60 * 60);
	eBreak   = CreateEvent(NULL,  TRUE, FALSE, NULL);
	eWake    = CreateEvent(NULL,  TRUE, FALSE, NULL);
	InitializeCriticalSection(lock + 0);
	InitializeCriticalSection(lock + 1);
	InitializeCriticalSection(lock + 2);
	v_t_copy = CreateThread(NULL, 0, copy_thread, NULL ,0, NULL);
	MH_Initialize();
	hook_init_ogl();
	hook_init_d3d9();
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		char tmp[32], *cptr;
		sprintf(tmp, "GLC_SM_%I32u_", GetCurrentProcessId());
		cptr = tmp + strlen(tmp);
		strcpy(cptr, "EVENT");
		event = CreateEventA(NULL, TRUE, FALSE, tmp);
		strcpy(cptr, "MUTEX0");
		mutex[0] = CreateMutexA(NULL, FALSE, tmp);
		strcpy(cptr, "MUTEX1");
		mutex[1] = CreateMutexA(NULL, FALSE, tmp);
		strcpy(cptr, "MUTEX2");
		mutex[2] = CreateMutexA(NULL, FALSE, tmp);
		strcpy(cptr, "FILE");
		file_map = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, GLC_MAX_SIZE * BUF_SIZE + 1, tmp);
		fm_buf = MapViewOfFile(file_map, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		hInst = hinstDLL;
		v_t_init = CreateThread(NULL, 0, init_thread, OpenThread(SYNCHRONIZE, FALSE, GetCurrentThreadId()), 0, NULL);
	} else if (fdwReason == DLL_PROCESS_DETACH) {
		WaitForSingleObject(v_t_init, INFINITE);
		CloseHandle(v_t_init);
		hook_exit_d3d9();
		hook_exit_ogl();
		MH_Uninitialize();
		SetEvent(eBreak);
		while (WaitForSingleObject(eWake, 10) == WAIT_OBJECT_0);
		SetEvent(eWake);
		WaitForSingleObject(v_t_copy, INFINITE);
		CloseHandle(v_t_copy);
		DeleteCriticalSection(lock + 2);
		DeleteCriticalSection(lock + 1);
		DeleteCriticalSection(lock + 0);
		CloseHandle(eWake);
		CloseHandle(eBreak);
		UnmapViewOfFile(fm_buf);
		CloseHandle(file_map);
		CloseHandle(mutex[2]);
		CloseHandle(mutex[1]);
		CloseHandle(mutex[0]);
		CloseHandle(event);
		DBG_PERF_FLUSH();
	}
	return TRUE;
}
