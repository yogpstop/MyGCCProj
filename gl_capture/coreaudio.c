#define COBJMACROS 1
#include <stdio.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

const CLSID _CLSID_MMDeviceEnumerator = { 0xbcde0395, 0xe52f, 0x467c, { 0x8e,0x3d, 0xc4,0x57,0x92,0x91,0x69,0x2e } };
const IID _IID_IMMDeviceEnumerator = { 0xa95664d2, 0x9614, 0x4f35, { 0xa7,0x46, 0xde,0x8d,0xb6,0x36,0x17,0xe6 } };
const IID _IID_IAudioClient = { 0x1cb9ad4c, 0xdbfa, 0x4c32, { 0xb1,0x78, 0xc2,0xf5,0x68,0xa7,0x03,0xb2 } };
const IID _IID_IAudioCaptureClient = { 0xc8adbd64, 0xe71e, 0x48a0, { 0xa4,0xde, 0x18,0x5c,0x39,0x5c,0xd3,0x17 } };
void coreaudio_capture() {
	CoInitialize(NULL);
	FILE *file = fopen("V:\\test.wav", "wb");
	WAVEFORMATEX *wfe;
	UINT32 buflen, pktlen, frames; DWORD flags;
	BYTE *buf;
	LARGE_INTEGER qpe;
	IMMDeviceEnumerator *mde;
	IMMDevice *md;
	IAudioClient *iac;
	IAudioCaptureClient *iacc;
	CoCreateInstance(&_CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, &_IID_IMMDeviceEnumerator, (void**) &mde);
	IMMDeviceEnumerator_GetDefaultAudioEndpoint(mde, eCapture, eConsole, &md);
	IMMDevice_Activate(md, &_IID_IAudioClient, CLSCTX_ALL, NULL, (void**) &iac);
	IAudioClient_GetMixFormat(iac, &wfe);
	IAudioClient_Initialize(iac, AUDCLNT_SHAREMODE_SHARED, 0, 0, 0, wfe, NULL);
	IAudioClient_GetBufferSize(iac, &buflen);
	IAudioClient_GetService(iac, &_IID_IAudioCaptureClient, (void**) &iacc);
	IAudioClient_Start(iac);
	printf("%I32u %I32u %I32u %I32u\n",wfe->nChannels,wfe->nSamplesPerSec,wfe->nBlockAlign,wfe->wBitsPerSample);
	while (1) {
		Sleep(1);
		IAudioCaptureClient_GetNextPacketSize(iacc, &pktlen);
		while (pktlen) {
			IAudioCaptureClient_GetBuffer(iacc, &buf, &frames, &flags,
					NULL, (UINT64*) &qpe.QuadPart);
			if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
				fwrite(buf, 1, frames * wfe->wBitsPerSample * wfe->nChannels / 8, file);
			//TODO
			}
			IAudioCaptureClient_ReleaseBuffer(iacc, frames);
			IAudioCaptureClient_GetNextPacketSize(iacc, &pktlen);
		}
	}
	IAudioClient_Stop(iac);
	CoTaskMemFree(wfe);
	IAudioCaptureClient_Release(iacc);
	IAudioClient_Release(iac);
	IMMDevice_Release(md);
	IMMDeviceEnumerator_Release(mde);
	fclose(file);
	CoUninitialize();
}
int main() {
	coreaudio_capture();
	return 0;
}
