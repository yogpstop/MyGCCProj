#include <streams.h>
#include <windows.h>
#include <tchar.h>

#define FRAME_LEN 50000000 //20fps

#define SET_TEXT(b); if(yogpdsf_curmode&FLAG_##b)\
	SetWindowText(b,TEXT(#b));\
	else\
	SetWindowText(b,TEXT("NO_")TEXT(#b));

#define MY_CREATE_BUTTON(a,b,c); a = CreateWindow(TEXT("BUTTON"),NULL,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,b,c,150,23,hWnd,(HMENU)ID_##a,GetModuleHandle(NULL),NULL);

#define SWITCH(a); case ID_##a: { yogpdsf_curmode ^= FLAG_##a;\
	SET_TEXT(a);\
	return 0; }

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

class IYOGPSource {
public:
	virtual float* getImage() = 0;
	virtual void releaseImage() = 0;
};

class C_HDC : public IYOGPSource {
public:
	C_HDC(int inw,int inh,int outw,int outh,int inx,int iny);

	~C_HDC();

 	float* getImage();

	void releaseImage();
private:
	HDC hdc;
	HDC hcpt;
	HBITMAP bmpcpt;
	BITMAPINFOHEADER bmih;
	VOID *bitmap;
	int ow;
	int oh;
	int ix;
	int iy;
	float scale;
	float *a;
	float *b;
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
	AM_MEDIA_TYPE ammt;
	VIDEOINFO vidinfo;
	int m_iFrameNumber;
	IYOGPSource* test;
	float* cache;
};

void nearest_neighbor(unsigned int, unsigned int, float,
		float *, float *, unsigned int, unsigned int);

void permeate(unsigned int, unsigned int, unsigned int,
		unsigned int, float *, float *,
		unsigned int, unsigned int, float);

void unit_conv(float *, unsigned char *, unsigned int, bool);

void output(unsigned char *, float *, unsigned int);