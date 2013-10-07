#include <audioclient.h>
#include <mmdeviceapi.h>
#include "coreaudio.h"
#include "options.h"

#define REFTIMES_PER_MILLISEC 10000
#define PIF(hr); if(FAILED(hr))return hr;

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

static const PROPERTYKEY PKEY_Device_FriendlyName = {
		{ 0xa45c254e, 0xdf1c, 0x4efd,
			{ 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } }, 14};

typedef struct {
	AUDIO_DATA aud;
    REFERENCE_TIME sleep_time;
    IAudioClient *ac;
	IAudioClient *acd;
    IAudioCaptureClient *acc;
} device_t;

#define INIT_DEVICE(d, sf, ds, hr) { \
	WAVEFORMATEX *wf; \
	{ \
		/*IPropertyStore *ps; PROPVARIANT pv; \
		hr = d->OpenPropertyStore(STGM_READ, &ps); PIF(hr); \
		PropVariantInit(&pv); \
		hr = ps->GetValue(PKEY_Device_FriendlyName, &pv); PIF(hr); \
		int len = WideCharToMultiByte(CP_THREAD_ACP, \
								0,pv.pwszVal,-1,NULL,0,NULL,NULL); \
		ds.aud.name = (char*)malloc(len); \
		WideCharToMultiByte(CP_THREAD_ACP, \
					0,pv.pwszVal,-1,ds.aud.name,len,NULL,NULL); \
		hr = PropVariantClear(&pv); \
		PIF(hr); \
		ps->Release(); \
		LPWSTR buf; \
		hr = d->GetId(&buf); \
		PIF(hr); \
		len = WideCharToMultiByte(CP_THREAD_ACP, \
								0,buf,-1,NULL,0,NULL,NULL); \
		ds.aud.id = (char*)malloc(len); \
		WideCharToMultiByte(CP_THREAD_ACP, \
					0,buf,-1,ds.aud.id,len,NULL,NULL); \
		CoTaskMemFree(buf); */ \
	} \
    hr = d->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&ds.ac); \
	PIF(hr); hr = ds.ac->GetMixFormat(&wf); PIF(hr); \
	if(wf->nChannels!=AUDIO_CHANNELS || wf->nSamplesPerSec!=AUDIO_SAMPLE_RATE \
			|| wf->wBitsPerSample!=AUDIO_BIT_DEPTH) \
		return AUDCLNT_E_UNSUPPORTED_FORMAT; \
    hr = ds.ac->Initialize(AUDCLNT_SHAREMODE_SHARED, sf, 0, 0, wf, NULL); \
	PIF(hr); \
	if(sf & AUDCLNT_STREAMFLAGS_LOOPBACK) { \
		hr = d->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&ds.acd); \
		PIF(hr); \
		hr = ds.acd->Initialize(AUDCLNT_SHAREMODE_SHARED, \
			AUDCLNT_SESSIONFLAGS_DISPLAY_HIDE, 0, 0, wf, NULL); \
	} \
	CoTaskMemFree(wf); \
	hr = ds.ac->GetDevicePeriod(&ds.sleep_time, NULL); PIF(hr); \
	ds.sleep_time /= REFTIMES_PER_MILLISEC * 2; \
    hr = ds.ac->GetService(IID_IAudioCaptureClient, (void**)&ds.acc); \
	PIF(hr); }

#define START_DEVICE(ds, hr) \
	hr = ds.ac->Start(); PIF(hr); \
	if(ds.acd) { hr = ds.acd->Start(); PIF(hr); }

static DWORD WINAPI record_device(device_t *ds) {
	HRESULT hr;
    float *src;
    UINT32 src_len;
	UINT32 src_len_raw;
    DWORD flag;
	unsigned int dst_group = 0;
	unsigned int cur_src;
    while (!stop_flag) {
        hr = ds->acc->GetNextPacketSize(&src_len);
		PIF(hr);
		if(!src_len) Sleep(ds->sleep_time);
        hr = ds->acc->GetBuffer((BYTE**)&src, &src_len_raw, &flag, NULL, NULL);
		PIF(hr);
		if(src_len_raw==0)continue;
		src_len = src_len_raw * AUDIO_CHANNELS;

        if (flag & AUDCLNT_BUFFERFLAGS_SILENT) {
			while(ds->aud.len[dst_group]<=src_len) {
				src_len -= ds->aud.len[dst_group];
				ds->aud.len[dst_group] = 0;
				dst_group++;
				if(dst_group>=audio_data_len) dst_group = 0;
				if(!ds->aud.len[dst_group]) {
					stop_flag = TRUE;
				}
			}
			ds->aud.len[dst_group] -= src_len;
		} else {
			for(cur_src = 0;cur_src<src_len;cur_src++) {
				ds->aud.data[dst_group * samples_per_block +
					samples_per_block - ds->aud.len[dst_group]] = src[cur_src];
				if(ds->aud.len[dst_group] == 1) {
					ds->aud.len[dst_group] = 0;
					dst_group++;
					if(dst_group>=audio_data_len) dst_group = 0;
					if(!ds->aud.len[dst_group]) {
						stop_flag = TRUE;
					}
				} else ds->aud.len[dst_group]--;
			}
		}
        hr = ds->acc->ReleaseBuffer(src_len_raw);
		PIF(hr);
		if(flag & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY) {
			//ef = TRUE;
			//return ERROR_TIMEOUT;
		}
    }
    return 0;
}

#define EXIT_DEVICE(ds) \
	if(ds.acc) { ds.acc->Release(); ds.acc = NULL; } \
	if(ds.acd) { ds.acd->Stop(); ds.acd->Release(); ds.acd = NULL; } \
	if(ds.ac) { ds.ac->Stop(); ds.ac->Release(); ds.ac = NULL; }

static device_t *ds;
static HANDLE *ht;
static UINT devices, i;
static HRESULT hr;

int windows_core_audio_init(void) {
	IMMDeviceEnumerator *pEnum;
    IMMDevice *pDevice;
	IMMDeviceCollection *pCapture, *pRender;
	UINT cDevices, rDevices, i;
    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL,
		IID_IMMDeviceEnumerator, (void**)&pEnum);
	PIF(hr);
	hr = pEnum->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCapture);
	PIF(hr);
	hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pRender);
	PIF(hr);
	pEnum->Release();
	hr = pCapture->GetCount(&cDevices); PIF(hr);
	hr = pRender->GetCount(&rDevices); PIF(hr);
	devices = rDevices + cDevices;
	ds = (device_t *)malloc(sizeof(device_t)*(devices));
	memset(ds, 0, sizeof(device_t)*(devices));
	ht = (HANDLE *)malloc(sizeof(HANDLE)*(devices));
	memset(ht, 0, sizeof(HANDLE)*(devices));
	for(i=0; i<rDevices; i++) {
		hr = pRender->Item(i, &pDevice); PIF(hr);
		INIT_DEVICE(pDevice, AUDCLNT_STREAMFLAGS_LOOPBACK, ds[i], hr);
		pDevice->Release();
	}
	pRender->Release();
	for(i=0; i<cDevices; i++) {
		hr = pCapture->Item(i, &pDevice); PIF(hr);
		INIT_DEVICE(pDevice, 0, ds[i+rDevices], hr);
		pDevice->Release();
	}
	pCapture->Release();
	audio_buf_len += devices;
	audio_buf = (AUDIO_DATA**)realloc
				(audio_buf, sizeof(AUDIO_DATA*) * audio_buf_len);
	for(i=0; i<devices; i++) {
		INIT_AUDIO_DATA(ds[i].aud, hr);
		audio_buf[audio_buf_len - i - 1] = &ds[i].aud;
	}
	return 0;
}
int windows_core_audio_sync(void) {
	for(i=0; i<devices; i++) {
		START_DEVICE(ds[i],hr);
	}
	return 0;
}
THREAD_RET_TYPE windows_core_audio_loop(void* dummy) {
	for(i=0; i<devices; i++)
		ht[i] = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)record_device, &ds[i], 0, NULL);
	WaitForMultipleObjects(i, ht, TRUE, INFINITE);
	for(i=0; i<devices; i++) {
		EXIT_DEVICE(ds[i]);
		CloseHandle(ht[i]);
	}
	free(ds);
	free(ht);
	return THREAD_RETURN_SUCCESS;
}

