#include <streams.h>
#include <windows.h>
#include <tchar.h>

#define FRAME_LEN 50000000 //20fps

#define MUTEX_WORD TEXT("yogp_dsf_mtx")

#define SET_TEXT(b); if(yogpdsf_curmode&FLAG_##b)\
	SetWindowText(b,TEXT(#b));\
	else\
	SetWindowText(b,TEXT("NO_")TEXT(#b));

#define MY_CREATE_BUTTON(a,b,c); a = CreateWindow(TEXT("BUTTON"),NULL,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,b,c,150,23,hWnd,(HMENU)ID_##a,GetModuleHandle(NULL),NULL);

#define SWITCH(a); case ID_##a: { HANDLE hmut = OpenMutex(MUTEX_ALL_ACCESS,FALSE,MUTEX_WORD);\
	yogpdsf_curmode ^= FLAG_##a;\
	SET_TEXT(a);\
	ReleaseMutex(hmut); CloseHandle(hmut); return 0; }

#define FLAG_CLIENT 0x1
#define FLAG_TIMER 0x2
#define FLAG_WATCH 0x4
#define FLAG_TITLE 0x8
#define FLAG_LAYER 0x10
#define FLAG_START 0x20
#define FLAG_IMAGE 0x40

// {F9B48648-86B3-41c8-A251-9F0452C5130B}
static const GUID CLSID_YOGP_DSF = 
{ 0xf9b48648, 0x86b3, 0x41c8, { 0xa2, 0x51, 0x9f, 0x4, 0x52, 0xc5, 0x13, 0xb } };

int dsfCreateWindow(void);

class MainFilter : public CSource {
public:
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN,HRESULT*);
private:
	MainFilter(LPUNKNOWN,HRESULT*);
	~MainFilter();
	CSourceStream *m_pPin;
};

class MainPin : public CSourceStream, public IAMStreamConfig, public IKsPropertySet/*, public IAMPushSource*/ {
public:
	MainPin(HRESULT *,CSource *);
	~MainPin();
	DECLARE_IUNKNOWN;

	STDMETHODIMP NonDelegatingQueryInterface(REFIID id, void **self) {
		if (id == IID_IKsPropertySet) {
			return GetInterface(static_cast<IKsPropertySet*>(this), self);
		}
		if (id == IID_IAMStreamConfig) {
			return GetInterface(static_cast<IAMStreamConfig*>(this), self);
		}
		/*if (id == IID_IAMPushSource) {
			return GetInterface(static_cast<IAMPushSource*>(this), self);
		}*/
		return CSourceStream::NonDelegatingQueryInterface(id, self);
	}

	HRESULT FillBuffer(IMediaSample *);
	HRESULT DoBufferProcessingLoop(void);
	HRESULT DecideBufferSize(IMemAllocator *,ALLOCATOR_PROPERTIES *);
	HRESULT CheckMediaType(const CMediaType *);
	HRESULT GetMediaType(int, CMediaType *);
	STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q) {
		return E_FAIL;
	}
	STDMETHODIMP GetFormat(AM_MEDIA_TYPE **);
	STDMETHODIMP SetFormat(AM_MEDIA_TYPE *);
	STDMETHODIMP GetStreamCaps(int,AM_MEDIA_TYPE **,BYTE *);
	STDMETHODIMP GetNumberOfCapabilities(int *,int *);

	STDMETHODIMP Set(REFGUID,DWORD,LPVOID,DWORD,LPVOID,DWORD);
	STDMETHODIMP Get(REFGUID,DWORD,LPVOID,DWORD,LPVOID,DWORD,DWORD *);
	STDMETHODIMP QuerySupported(REFGUID,DWORD,DWORD *);

	/*STDMETHODIMP GetPushSourceFlags(ULONG *);
	STDMETHODIMP SetPushSourceFlags(ULONG);
	STDMETHODIMP GetMaxStreamOffset(REFERENCE_TIME *);
	STDMETHODIMP SetMaxStreamOffset(REFERENCE_TIME);
	STDMETHODIMP GetStreamOffset(REFERENCE_TIME *);
	STDMETHODIMP SetStreamOffset(REFERENCE_TIME);

	STDMETHODIMP GetLatency(REFERENCE_TIME *);*/
private:
	HDC hDesktopDC;
	HDC hCaptureDC;
	HBITMAP hCaptureBitmap;
	VOID *bitmap;
	AM_MEDIA_TYPE ammt;
	VIDEOINFO vidinfo;
	int m_iFrameNumber;
	HBITMAP hBmp;
	HBITMAP hBmp2;
	HDC toplayer;
	HDC background;
	HANDLE hMutex;
};