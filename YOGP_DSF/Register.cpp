#include "YOGP_DSF.h"

static const AMOVIESETUP_MEDIATYPE sudPinTypes = {
	&MEDIATYPE_Video,
	&MEDIASUBTYPE_RGB32
};

static const AMOVIESETUP_PIN sudOutputPin = {
	NULL,
	FALSE,
	TRUE,
	FALSE,
	FALSE,
	&CLSID_NULL,
	NULL,
	1,
	&sudPinTypes
};


static const REGFILTERPINS2 sudOutputPin2 = {
	REG_PINFLAG_B_OUTPUT,
	1,
	1,
	&sudPinTypes,
	0,
	NULL,
	&PIN_CATEGORY_CAPTURE
};

static const REGFILTER2 rf2FilterReg = {
	2,
	MERIT_DO_NOT_USE,
	1,
	(REGFILTERPINS*)&sudOutputPin2
};
const AMOVIESETUP_FILTER sudPushSource = {
	&CLSID_YOGP_DSF,
	TEXT("YOGP_DSF"),
	MERIT_DO_NOT_USE,
	1,
	&sudOutputPin
};

CFactoryTemplate g_Templates[] = {
	{ TEXT("YOGP_DSF")
	, &CLSID_YOGP_DSF
	,MainFilter::CreateInstance
	, NULL
	, &sudPushSource }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

STDAPI DllRegisterServer() {
	HRESULT hr;
	IFilterMapper2 *pFM2 = NULL;

	hr = AMovieDllRegisterServer2(TRUE);
	if(FAILED(hr)) return hr;

	hr = CoCreateInstance(CLSID_FilterMapper2,NULL,CLSCTX_INPROC_SERVER,IID_IFilterMapper2,(void **)&pFM2);
	if(FAILED(hr)) return hr;

	hr = pFM2->RegisterFilter(CLSID_YOGP_DSF,TEXT("YOGP_DSF"),NULL,&CLSID_VideoInputDeviceCategory,TEXT("YOGP_DSF"),&rf2FilterReg);

	pFM2->Release();

	return hr;

}

STDAPI DllUnregisterServer() {
	HRESULT hr;
	IFilterMapper2 *pFM2 = NULL;

	hr = AMovieDllRegisterServer2(FALSE);
	if(FAILED(hr)) return hr;

	hr = CoCreateInstance(CLSID_FilterMapper2,NULL,CLSCTX_INPROC_SERVER,IID_IFilterMapper2,(void **)&pFM2);
	if (FAILED(hr)) return hr;

	hr = pFM2->UnregisterFilter(&CLSID_VideoInputDeviceCategory,TEXT("YOGP_DSF"),CLSID_YOGP_DSF);

	pFM2->Release();

	return hr;

}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule,DWORD  dwReason,LPVOID lpReserved) {
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
