#include "YOGP_DSF.h"

#ifdef _DEBUG
#pragma comment (lib,"strmbasd.lib")
#else
#pragma comment (lib,"strmbase.lib")
#endif
#pragma comment (lib,"winmm.lib")

DWORD yogpdsf_curmode;
FILETIME starttime;

CUnknown * WINAPI MainFilter::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr) {
	CUnknown *punk = new MainFilter(lpunk, phr);
	if(punk == NULL)
		if(phr)
			*phr = E_OUTOFMEMORY;
	return punk;

}

RECT render;
HFONT font;

MainFilter::MainFilter(LPUNKNOWN lpunk,HRESULT *phr) :
CSource(TEXT("YOGP_DSF"),lpunk,CLSID_YOGP_DSF,phr){
	m_pPin = new MainPin(phr, this);
	//m_paStreams = &m_pPin;
	//m_iPins = 1;
	if (phr){
		if (m_pPin == NULL)
			*phr = E_OUTOFMEMORY;
		else
			*phr = S_OK;
	}  
}

MainFilter::~MainFilter(){
	delete m_pPin;
}

HRESULT MainPin::FillBuffer(IMediaSample *pSample) {
	BYTE *p;
	HRESULT hr;
	CheckPointer(pSample,E_POINTER);
	hr = pSample->GetPointer(&p);
	if(FAILED(hr))
		return hr;
	CheckPointer(p,E_POINTER);
	test->getImage(p);
	pSample->SetActualDataLength(vidinfo.bmiHeader.biSizeImage);
	return NOERROR;
}

MainPin::MainPin(HRESULT *phr,CSource *pms):
CSourceStream(TEXT("YOGP_DSF"),phr,pms,TEXT("Capture")),
m_iFrameNumber(0){
	ZeroMemory(&vidinfo, sizeof(vidinfo));
	vidinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	vidinfo.bmiHeader.biWidth = 640;
	vidinfo.bmiHeader.biHeight = 360;
	vidinfo.bmiHeader.biPlanes = 1;
	vidinfo.bmiHeader.biBitCount = 24;
	vidinfo.bmiHeader.biCompression = BI_RGB;
	vidinfo.bmiHeader.biSizeImage = GetBitmapSize(&vidinfo.bmiHeader);
	vidinfo.AvgTimePerFrame = FRAME_LEN;
	ZeroMemory(&ammt,sizeof(ammt));
	ammt.majortype = MEDIATYPE_Video;
	ammt.subtype = MEDIASUBTYPE_RGB24;
	ammt.lSampleSize = vidinfo.bmiHeader.biSizeImage;
	ammt.bFixedSizeSamples = TRUE;
	ammt.bTemporalCompression = FALSE;
	ammt.formattype = FORMAT_VideoInfo;
	ammt.cbFormat = sizeof(VIDEOINFO);
	ammt.pbFormat = (BYTE *)&vidinfo;
	test = new C_HDC();
}

MainPin::~MainPin() {
	delete test;
}

HRESULT MainPin::CheckMediaType(const CMediaType *pMediaType) {
	CheckPointer(pMediaType,E_POINTER);
	if(*pMediaType->Type()!=MEDIATYPE_Video)
		return E_FAIL;
	if(*pMediaType->Subtype()!=GetBitmapSubtype(&vidinfo.bmiHeader) && *pMediaType->Subtype()!=MEDIASUBTYPE_NULL)
		return E_FAIL;
	if(*pMediaType->FormatType()!=FORMAT_VideoInfo)
		return E_FAIL;
	BITMAPINFOHEADER *bih = &((VIDEOINFOHEADER *)pMediaType->Format())->bmiHeader;
	if(bih->biBitCount!=vidinfo.bmiHeader.biBitCount)
		return E_FAIL;
	if(bih->biWidth!=vidinfo.bmiHeader.biWidth)
		return E_FAIL;
	if(bih->biHeight!=vidinfo.bmiHeader.biHeight)
		return E_FAIL;
	if(bih->biPlanes!=vidinfo.bmiHeader.biPlanes)
		return E_FAIL;
	if(bih->biCompression!=vidinfo.bmiHeader.biCompression)
		return E_FAIL;
	return S_OK;
}

HRESULT MainPin::GetMediaType(int i, CMediaType *pMediaType) {
	if(i<0)
		return E_INVALIDARG;
	if(i>0)
		return VFW_S_NO_MORE_ITEMS;
	CheckPointer(pMediaType,E_POINTER);
	pMediaType->Set(ammt);
	return S_OK;
}

HRESULT MainPin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ap) {
	CheckPointer(pAlloc,E_POINTER);
	CheckPointer(ap,E_POINTER);

	HRESULT hr = NOERROR;

	ap->cBuffers = 1;
	ap->cbBuffer = vidinfo.bmiHeader.biSizeImage;

	ALLOCATOR_PROPERTIES actual;
	hr = pAlloc->SetProperties(ap,&actual);

	if(FAILED(hr))
		return hr;

	if(actual.cbBuffer < ap->cbBuffer)
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP MainPin::GetFormat(AM_MEDIA_TYPE **a){
	CheckPointer(a,E_POINTER);
	*a = CreateMediaType(&ammt);
	return S_OK;
}
STDMETHODIMP MainPin::SetFormat(AM_MEDIA_TYPE *a){
	return E_UNEXPECTED;
}
STDMETHODIMP MainPin::GetStreamCaps(int i,AM_MEDIA_TYPE **a,BYTE *b){
	CheckPointer(a,E_POINTER);
	CheckPointer(b,E_POINTER);
    if(i>0)
		return S_FALSE;
	if(i<0)
		return E_INVALIDARG;
	VIDEO_STREAM_CONFIG_CAPS *caps = (VIDEO_STREAM_CONFIG_CAPS*)b;
	ZeroMemory(caps, sizeof(VIDEO_STREAM_CONFIG_CAPS));
	caps->VideoStandard = 0;
	caps->guid = FORMAT_VideoInfo;
	caps->CropAlignX = 4;
	caps->CropAlignY = 1;
	caps->CropGranularityX = vidinfo.bmiHeader.biWidth;
	caps->CropGranularityY = abs(vidinfo.bmiHeader.biHeight);
	caps->OutputGranularityX = vidinfo.bmiHeader.biWidth;
	caps->OutputGranularityY = abs(vidinfo.bmiHeader.biHeight);
	caps->InputSize.cx = vidinfo.bmiHeader.biWidth;
	caps->InputSize.cy = abs(vidinfo.bmiHeader.biHeight);
	caps->MinOutputSize.cx = 640;
	caps->MinOutputSize.cy = 360;
	caps->MaxOutputSize.cx = 1920;
	caps->MaxOutputSize.cy = 1080;
	caps->MinCroppingSize = caps->InputSize;
	caps->MaxCroppingSize = caps->InputSize;
	caps->MinBitsPerSecond = 1;
	caps->MaxBitsPerSecond = 8294400;
	caps->MinFrameInterval = 500000;
	caps->MaxFrameInterval = 333334;
	caps->StretchTapsX = 0;
	caps->StretchTapsY = 0;
	caps->ShrinkTapsX = 0;
	caps->ShrinkTapsY = 0;
	*a = CreateMediaType(&ammt);
	return S_OK;
}
STDMETHODIMP MainPin::GetNumberOfCapabilities(int *i,int *j){
	CheckPointer(i,E_POINTER);
	CheckPointer(j,E_POINTER);
	*i = 1;
	*j = sizeof(VIDEOINFO);
	return S_OK;
}

STDMETHODIMP MainPin::Set(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData, DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData)
{
	return E_NOTIMPL;
}

STDMETHODIMP MainPin::Get(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData, DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData, DWORD *pcbReturned)
{
	if (guidPropSet != AMPROPSETID_Pin)
		return E_PROP_SET_UNSUPPORTED;
	if (dwPropID != AMPROPERTY_PIN_CATEGORY)
		return E_PROP_ID_UNSUPPORTED;
	if (pPropData == NULL && pcbReturned == NULL)
		return E_POINTER;
	if (pcbReturned)
		*pcbReturned = sizeof(GUID);
	if (pPropData == NULL)
		return S_OK;
	if (cbPropData < sizeof(GUID))
		return E_UNEXPECTED;
	*(GUID *)pPropData = PIN_CATEGORY_CAPTURE;

	return S_OK;
}

STDMETHODIMP MainPin::QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport)
{
	if (guidPropSet != AMPROPSETID_Pin)
		return E_PROP_SET_UNSUPPORTED;
	if (dwPropID != AMPROPERTY_PIN_CATEGORY)
		return E_PROP_ID_UNSUPPORTED;
	if (pTypeSupport)
		*pTypeSupport = KSPROPERTY_SUPPORT_GET; 

	return S_OK;
}

HRESULT MainPin::DoBufferProcessingLoop(void) {
	Command command;
	IMediaSample *sample;
	HRESULT result;
	LARGE_INTEGER nFreq, nAfter, nCurrent, nAmount;
	int error;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nCurrent);
	error = (int)(nFreq.QuadPart % 20);
	nAmount.QuadPart = (nFreq.QuadPart - error) / 20;
	m_iFrameNumber = 0;

	do {
		while (!CheckRequest(&command)) {
			result = GetDeliveryBuffer(&sample, NULL, NULL, 0);
			if (FAILED(result)) {
				::Sleep(1);
				continue;
			}

			{
				m_iFrameNumber++;

				REFERENCE_TIME rtStart = m_iFrameNumber * FRAME_LEN;
				REFERENCE_TIME rtStop  = rtStart + FRAME_LEN;
				sample->SetTime(&rtStart, &rtStop);

				nCurrent.QuadPart += nAmount.QuadPart;
				if(!(m_iFrameNumber % 20) && m_iFrameNumber)
					nCurrent.QuadPart += error;
			}

			QueryPerformanceCounter(&nAfter);
			if(nAfter.QuadPart>nCurrent.QuadPart)
				continue;			
			
			sample->SetSyncPoint(TRUE);
			result = FillBuffer(sample);

			if (result == S_OK) {
				result = Deliver(sample);
				sample->Release();
				if (result != S_OK)
					return S_OK;
			} else if (result == S_FALSE) {
				sample->Release();
				DeliverEndOfStream();
				return S_OK;
			} else {
				sample->Release();
				DeliverEndOfStream();
				m_pFilter->NotifyEvent(EC_ERRORABORT, result, 0);
				return result;
			}

			QueryPerformanceCounter(&nAfter);
			if(nAfter.QuadPart>nCurrent.QuadPart)
				continue;
			::Sleep((DWORD)(1000*(nCurrent.QuadPart-nAfter.QuadPart)/nFreq.QuadPart));
		}

		if (command == CMD_RUN || command == CMD_PAUSE) {
			Reply(S_OK);
		} else if (command != CMD_STOP) {
			Reply((DWORD) E_UNEXPECTED);
		}
	} while (command != CMD_STOP);

	return S_FALSE;
}