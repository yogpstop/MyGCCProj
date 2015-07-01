#include <d3d9.h>
#include "MinHook.h"
#include "main.h"
#include "dbg.h"
#include "hook.h"

HANDLE dll;
typedef IDirect3D9* __stdcall (*DIRECT3DCREATE9PROC)(UINT);
typedef HRESULT __stdcall (* PRESENTPROC)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef HRESULT __stdcall (*   RESETPROC)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT __stdcall (*ENDSCENEPROC)(IDirect3DDevice9*);
static  PRESENTPROC tPresent;
static  PRESENTPROC oPresent;
//static    RESETPROC tReset;
//static    RESETPROC oReset;
//static ENDSCENEPROC tEndScene;
//static ENDSCENEPROC oEndScene;

static IDirect3DSurface9 **rts;
static IDirect3DSurface9 **oss;
static int bitdepth = 4;//TODO bit depth
static int height = -1;

//FIXME check this
static HRESULT __stdcall dPresent(IDirect3DDevice9 *this, const RECT *src_rect,
		const RECT *dst_rect, HWND dst_window_override, const RGNDATA *dirty_region) {
	DBG_PERF(11, 0);
	static int idx = 2;
	if (height < 0) {
		IDirect3DSurface9 *bb; D3DSURFACE_DESC sd;
		this->lpVtbl->GetBackBuffer(this, 0, 0, D3DBACKBUFFER_TYPE_MONO, &bb);
		bb->lpVtbl->GetDesc(bb, &sd);
		bb->lpVtbl->Release(bb);
		height = sd.Height;
		rts = malloc(sizeof(IDirect3DSurface9*)*3);
		oss = malloc(sizeof(IDirect3DSurface9*)*3);
		this->lpVtbl->CreateRenderTarget(this, sd.Width, sd.Height, sd.Format, sd.MultiSampleType, sd.MultiSampleQuality, FALSE, rts + 0, NULL);
		this->lpVtbl->CreateRenderTarget(this, sd.Width, sd.Height, sd.Format, sd.MultiSampleType, sd.MultiSampleQuality, FALSE, rts + 1, NULL);
		this->lpVtbl->CreateRenderTarget(this, sd.Width, sd.Height, sd.Format, sd.MultiSampleType, sd.MultiSampleQuality, FALSE, rts + 2, NULL);
		this->lpVtbl->CreateOffscreenPlainSurface(this, sd.Width, sd.Height, sd.Format, D3DPOOL_SYSTEMMEM, oss + 0, NULL);
		this->lpVtbl->CreateOffscreenPlainSurface(this, sd.Width, sd.Height, sd.Format, D3DPOOL_SYSTEMMEM, oss + 1, NULL);
		this->lpVtbl->CreateOffscreenPlainSurface(this, sd.Width, sd.Height, sd.Format, D3DPOOL_SYSTEMMEM, oss + 2, NULL);
	}
	DBG_PERF(11, 1);
	QueryPerformanceCounter(&qpc_real);
	if (qpc_real.QuadPart >= qpc_target.QuadPart) {
		DBG_PERF(11, 2);
		if (WaitForSingleObject(eWake, 0) == WAIT_TIMEOUT) {
			DBG_PERF(11, 3);
			if (TryEnterCriticalSection(lock + idx)) {
				qpc_target.QuadPart += interval;
				if (qpc_target.QuadPart < qpc_real.QuadPart) qpc_target.QuadPart = qpc_real.QuadPart;
				qpc[idx] = qpc_real;
				IDirect3DSurface9 *bb; D3DLOCKED_RECT lr;
				oss[idx]->lpVtbl->UnlockRect(oss[idx]);
				this->lpVtbl->GetBackBuffer(this, 0, 0, D3DBACKBUFFER_TYPE_MONO, &bb);
				this->lpVtbl->StretchRect(this, bb, NULL, rts[idx], NULL, D3DTEXF_NONE);
				bb->lpVtbl->Release(bb);
				DBG_PERF(11, 4);
				if (--idx < 0) idx = 2;
				this->lpVtbl->GetRenderTargetData(this, rts[idx], oss[idx]);
				LeaveCriticalSection(lock + idx);
				DBG_PERF(11, 5);
				if (--idx < 0) idx = 2;
				oss[idx]->lpVtbl->LockRect(oss[idx], &lr, NULL, D3DLOCK_READONLY);
				rect[idx].right = lr.Pitch / bitdepth;
				rect[idx].bottom = height;
				vatex = lr.pBits;
				gidx = idx;
				SetEvent(eWake);
			}
		}
	}
	DBG_PERF(11, 6);
	DBG_PERF_INC(11);
	return oPresent(this, src_rect, dst_rect, dst_window_override, dirty_region);
}

static HWND dwnd;

static DWORD WINAPI WndThread(LPVOID hInst) {
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style = CS_OWNDC;
	wc.hInstance = hInst;
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = "GLC_DUMMY_WNDCLASS";
	RegisterClass(&wc);
	CreateWindow(wc.lpszClassName, NULL, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0, 1, 1, NULL, NULL, hInst, NULL);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg); DispatchMessage(&msg);
	}
	return 0;
}

void hook_init_d3d9() {
	CreateThread(NULL, 0, WndThread, NULL, 0, NULL);//TODO hInst
	DBG_PERF_INIT(11, 7, GLC_FPS * 60 * 60);
	dll = GetModuleHandleA("d3d9.dll");
	if (!dll) return;
	DIRECT3DCREATE9PROC create = (DIRECT3DCREATE9PROC) GetProcAddress(dll, "Direct3DCreate9");
	IDirect3D9 *obj = create(D3D_SDK_VERSION);
	IDirect3DDevice9 *device;
	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));
	pp.Windowed             = 1;
	pp.SwapEffect           = D3DSWAPEFFECT_FLIP;
	pp.BackBufferFormat     = D3DFMT_A8R8G8B8;
	pp.BackBufferCount      = 1;
	pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	pp.hDeviceWindow = dwnd;
	obj->lpVtbl->CreateDevice(obj, D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, pp.hDeviceWindow,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_NOWINDOWCHANGES, &pp, &device);
	tPresent = device->lpVtbl->Present;
	MH_CreateHook(tPresent, dPresent, (void**) &oPresent);
	MH_EnableHook(tPresent);
	device->lpVtbl->Release(device);
}

void hook_exit_d3d9() {
	if (!dll) return;
	MH_DisableHook(tPresent);
	MH_RemoveHook(tPresent);
}
