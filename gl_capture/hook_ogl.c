#include <windows.h>
#include "MinHook.h"
#include "main.h"
#include "hook.h"
#include "dbg.h"

static HANDLE dll;
typedef unsigned char GLboolean;
typedef unsigned int  GLenum;
typedef unsigned int  GLuint;
typedef          int  GLint;
typedef          int  GLsizei;
typedef          void GLvoid;
typedef ptrdiff_t     GLsizeiptr;
#define GL_UNSIGNED_BYTE     0x1401
#define GL_BACK              0x0405
#define GL_BGRA              0x80E1
#define GL_READ_ONLY         0x88B8
#define GL_STREAM_READ       0x88E1
#define GL_PIXEL_PACK_BUFFER 0x88EB
typedef void      (__stdcall *  GLBUFFERDATAPROC) (GLenum, GLsizeiptr, const void*, GLenum);
typedef void      (__stdcall *  GLGENBUFFERSPROC) (GLsizei, GLuint*);
typedef void      (__stdcall *  GLBINDBUFFERPROC) (GLenum, GLuint);
typedef void*     (__stdcall *   GLMAPBUFFERPROC) (GLenum, GLenum);
typedef GLboolean (__stdcall * GLUNMAPBUFFERPROC) (GLenum);
typedef void      (__stdcall *  GLREADBUFFERPROC) (GLenum);
typedef void      (__stdcall *  GLREADPIXELSPROC) (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);
typedef PROC      (__stdcall * WGLGETPROCADDRESSPROC) (LPCSTR);
typedef BOOL      (__stdcall *    WGLSWAPBUFFERSPROC) (HDC);
static GLGENBUFFERSPROC  glGenBuffers;
static GLBINDBUFFERPROC  glBindBuffer;
static GLBUFFERDATAPROC  glBufferData;
static GLUNMAPBUFFERPROC glUnmapBuffer;
static GLMAPBUFFERPROC   glMapBuffer;
static GLREADBUFFERPROC  glReadBuffer;
static GLREADPIXELSPROC  glReadPixels;
static WGLGETPROCADDRESSPROC _wglGetProcAddress;
static WGLSWAPBUFFERSPROC target;
static WGLSWAPBUFFERSPROC orig;

static GLuint gltex[2] = {0, 0};

//FIXME check hdc
static BOOL APIENTRY detour(HDC hdc) {
	DBG_PERF(1, 0);
	static int idx = 0;
	if (gltex[1] == 0) {
		glGenBuffers  = (GLGENBUFFERSPROC)  _wglGetProcAddress("glGenBuffers");
		glBindBuffer  = (GLBINDBUFFERPROC)  _wglGetProcAddress("glBindBuffer");
		glBufferData  = (GLBUFFERDATAPROC)  _wglGetProcAddress("glBufferData");
		glUnmapBuffer = (GLUNMAPBUFFERPROC) _wglGetProcAddress("glUnmapBuffer");
		glMapBuffer   = (GLMAPBUFFERPROC)   _wglGetProcAddress("glMapBuffer");
		glGenBuffers(2, gltex);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, gltex[0]);
		glBufferData(GL_PIXEL_PACK_BUFFER, GLC_MAX_PSIZE, NULL, GL_STREAM_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, gltex[1]);
		glBufferData(GL_PIXEL_PACK_BUFFER, GLC_MAX_PSIZE, NULL, GL_STREAM_READ);
	}
	DBG_PERF(1, 1);
	QueryPerformanceCounter(&qpc_real);
	if (qpc_real.QuadPart >= qpc_target.QuadPart) {
		DBG_PERF(1, 2);
		if (WaitForSingleObject(eWake, 0) == WAIT_TIMEOUT) {
			DBG_PERF(1, 3);
			if (TryEnterCriticalSection(lock + idx)) {
				qpc_target.QuadPart += interval;
				if (qpc_target.QuadPart < qpc_real.QuadPart) qpc_target.QuadPart = qpc_real.QuadPart;
				qpc[idx] = qpc_real;
				GetClientRect(WindowFromDC(hdc), rect + idx);
				DBG_PERF(1, 4);
				glReadBuffer(GL_BACK);
				glBindBuffer(GL_PIXEL_PACK_BUFFER, gltex[idx]);
				DBG_PERF(1, 5);
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
				DBG_PERF(1, 6);
				glReadPixels(0, 0, rect[idx].right, rect[idx].bottom, GL_BGRA, GL_UNSIGNED_BYTE, 0);
				DBG_PERF(1, 7);
				LeaveCriticalSection(lock + idx);
				DBG_PERF(1, 8);
				idx ^= 1;
				glBindBuffer(GL_PIXEL_PACK_BUFFER, gltex[idx]);
				vatex = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
				gidx = idx;
				SetEvent(eWake);
			}
		}
	}
	DBG_PERF(1, 9);
	DBG_PERF_INC(1);
	return (*orig)(hdc);
}

void hook_init_ogl() {
	DBG_PERF_INIT(1, 10, GLC_FPS * 60 * 60);
	dll = GetModuleHandleA("opengl32.dll");
	if (!dll) return;
	target = (WGLSWAPBUFFERSPROC) GetProcAddress(dll, "wglSwapBuffers");
	_wglGetProcAddress = (WGLGETPROCADDRESSPROC) GetProcAddress(dll, "wglGetProcAddress");
	glReadBuffer  = (GLREADBUFFERPROC)  GetProcAddress(dll, "glReadBuffer");
	glReadPixels  = (GLREADPIXELSPROC)  GetProcAddress(dll, "glReadPixels");
	MH_CreateHook(target, detour, (void**) &orig);
	MH_EnableHook(target);
}
void hook_exit_ogl() {
	if (!dll) return;
	MH_DisableHook(target);
	MH_RemoveHook(target);
}
