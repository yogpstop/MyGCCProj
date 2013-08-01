#include <streams.h>
#include <windows.h>
#include <tchar.h>

#define FRAME_LEN 50000000 //20fps

// {F9B48648-86B3-41c8-A251-9F0452C5130B}
static const GUID CLSID_YOGP_DSF = 
{ 0xf9b48648, 0x86b3, 0x41c8, { 0xa2, 0x51, 0x9f, 0x4, 0x52, 0xc5, 0x13, 0xb } };

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
	virtual void getImage(unsigned char*) = 0;
	virtual void releaseImage() = 0;
};

class C_HDC : public IYOGPSource {
public:
	C_HDC();

	~C_HDC();

	void getImage(unsigned char*);

	void releaseImage();
private:
	HDC hdc;
	HDC hcpt;
	HBITMAP bmpcpt;
	BITMAPINFOHEADER bmih;
	unsigned char *bitmap;
};

class MainPin : public CSourceStream, public IAMStreamConfig, public IKsPropertySet {
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
private:
	AM_MEDIA_TYPE ammt;
	VIDEOINFO vidinfo;
	int m_iFrameNumber;
	IYOGPSource* test;
};

void nearest_neighbor(unsigned int, unsigned int, float,
					  float *, float *, unsigned int, unsigned int);

void nearest_neighbor(unsigned int, unsigned int, float, float*, unsigned char*, unsigned int, unsigned int, bool);

void permeate(unsigned int, unsigned int, unsigned int,
			  unsigned int, float *, float *,
			  unsigned int, unsigned int, float);

void input(float *, unsigned char *, unsigned int, bool);

void output(unsigned char *, float *, unsigned int);