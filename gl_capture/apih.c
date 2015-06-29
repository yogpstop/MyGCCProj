#include <stdio.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "MinHook.h"
#include "main.h"
#include "dbg.h"

typedef BOOL APIENTRY (*wsbt)(HDC);
static wsbt target;
static wsbt orig;

static HANDLE file_map;
static void *fm_buf;
static HANDLE mutex[2], event;

static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLBUFFERDATAPROC glBufferData;
static PFNGLUNMAPBUFFERPROC glUnmapBuffer;
static PFNGLMAPBUFFERPROC glMapBuffer;

static HANDLE eBreak;

static HANDLE eWake;
static int gidx;
static void *vatex;

static CRITICAL_SECTION lock[2];
static LARGE_INTEGER qpc[2];
static RECT rect[2];

static DWORD WINAPI copy_thread(LPVOID lpp) {
	int idx; void *inp, *outp;
	while (WaitForSingleObject(eWake, INFINITE) == WAIT_OBJECT_0) {
		DBG_PERF(2, 0);
		if (WaitForSingleObject(eBreak, 0) == WAIT_OBJECT_0) break;
		DBG_PERF(2, 1);
		idx = gidx;
		inp = vatex;
		DBG_PERF(2, 2);
		EnterCriticalSection(lock + idx);
		DBG_PERF(2, 3);
		ResetEvent(eWake);
		outp = fm_buf + idx * GLC_MAX_SIZE;
		DBG_PERF(2, 4);
		if (WaitForSingleObject(event, 0) != WAIT_OBJECT_0) {
			DBG_PERF(2, 5);
			if (WaitForSingleObject(mutex[idx], 0) == WAIT_OBJECT_0) {
				DBG_PERF(2, 6);
				*((LONGLONG*)outp) = qpc[idx].QuadPart;
				*((uint16_t*)(outp + sizeof(LONGLONG))) = rect[idx].right;
				*((uint16_t*)(outp + sizeof(LONGLONG) + sizeof(uint16_t))) = rect[idx].bottom;
				DBG_PERF(2, 7);
				memcpy(outp + GLC_HDR_SIZE, inp, rect[idx].right * rect[idx].bottom * GLC_COLORDEPTH);
				DBG_PERF(2, 8);
				ReleaseMutex(mutex[idx]);
			}
			*((uint8_t*)fm_buf + GLC_MAX_SIZE * 2) = idx;
			SetEvent(event);
		}
		LeaveCriticalSection(lock + idx);
		DBG_PERF(2, 9);
		DBG_PERF_INC(2);
	}
	return 0;
}
static BOOL APIENTRY detour(HDC hdc) {
	DBG_PERF(1, 0);
	static int interval;
	static int idx = 0;
	static HANDLE thread = INVALID_HANDLE_VALUE;
	static GLuint gltex[2];
	static LARGE_INTEGER qpc_real, qpc_target;
	if (thread == INVALID_HANDLE_VALUE) {
		glGenBuffers  = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
		glBindBuffer  = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer");
		glBufferData  = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
		glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) wglGetProcAddress("glUnmapBuffer");
		glMapBuffer   = (PFNGLMAPBUFFERPROC) wglGetProcAddress("glMapBuffer");
		eBreak   = CreateEvent(NULL,  TRUE, FALSE, NULL);
		eWake    = CreateEvent(NULL,  TRUE, FALSE, NULL);
		InitializeCriticalSection(lock);
		InitializeCriticalSection(lock + 1);
		glGenBuffers(2, gltex);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, gltex[0]);
		glBufferData(GL_PIXEL_PACK_BUFFER, GLC_MAX_PSIZE, NULL, GL_STREAM_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, gltex[1]);
		glBufferData(GL_PIXEL_PACK_BUFFER, GLC_MAX_PSIZE, NULL, GL_STREAM_READ);
		thread = CreateThread(NULL, 0, copy_thread, NULL ,0, NULL);
		QueryPerformanceFrequency(&qpc_real);
		interval = qpc_real.QuadPart / GLC_FPS;
		QueryPerformanceCounter(&qpc_target);
	}
	DBG_PERF(1, 1);
	QueryPerformanceCounter(&qpc_real);
	if (qpc_real.QuadPart >= qpc_target.QuadPart) {
		DBG_PERF(1, 2);
		glReadBuffer(GL_BACK);
		if (WaitForSingleObject(eWake, 0) != WAIT_OBJECT_0) {
			DBG_PERF(1, 3);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, gltex[idx]);
			if (TryEnterCriticalSection(lock + idx)) {
				DBG_PERF(1, 4);
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
				qpc[idx] = qpc_real;
				GetClientRect(WindowFromDC(hdc), rect + idx);
				glReadPixels(0, 0, rect[idx].right, rect[idx].bottom, GL_BGRA, GL_UNSIGNED_BYTE, 0);
				LeaveCriticalSection(lock + idx);
				qpc_target.QuadPart += interval;
				if (qpc_target.QuadPart < qpc_real.QuadPart) qpc_target.QuadPart = qpc_real.QuadPart;
			}
			DBG_PERF(1, 5);
			// --------------------------------------------------------------
			idx ^= 1;
			glBindBuffer(GL_PIXEL_PACK_BUFFER, gltex[idx]);
			vatex = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
			gidx = idx;
			SetEvent(eWake);
		}
	}
	DBG_PERF(1, 6);
	DBG_PERF_INC(1);
	return (*orig)(hdc);
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		target = (wsbt) GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
		DBG_PERF_INIT(1, 7, GLC_FPS * 60 * 60);
		DBG_PERF_INIT(2, 10, GLC_FPS * 60 * 60);
		char tmp[32], *cptr;
		sprintf(tmp, "GLC_SM_%I32u_", GetCurrentProcessId());
		cptr = tmp + strlen(tmp);
		strcpy(cptr, "EVENT");
		event = CreateEventA(NULL, TRUE, FALSE, tmp);
		strcpy(cptr, "MUTEX0");
		mutex[0] = CreateMutexA(NULL, FALSE, tmp);
		strcpy(cptr, "MUTEX1");
		mutex[1] = CreateMutexA(NULL, FALSE, tmp);
		strcpy(cptr, "FILE");
		file_map = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, GLC_MAX_SIZE * 2 + 1, tmp);
		fm_buf = MapViewOfFile(file_map, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		MH_Initialize();
		MH_CreateHook(target, detour, (void**) &orig);
		MH_EnableHook(target);
	} else if (fdwReason == DLL_PROCESS_DETACH) {
		MH_DisableHook(target);
		MH_RemoveHook(target);
		MH_Uninitialize();
		UnmapViewOfFile(fm_buf);
		CloseHandle(file_map);
		CloseHandle(mutex[1]);
		CloseHandle(mutex[0]);
		CloseHandle(event);
		DBG_PERF_FLUSH();
	}
	return TRUE;
}
