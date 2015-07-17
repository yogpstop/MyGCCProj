#ifdef SNDmmdev
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include "main.h"
#define RATE 44100
#define PERIOD_SIZE 48
#define PERIODS 30
#define REFTIME(a) ((unsigned long long)(a) * 10000000 / 44100)
#define CHANNELS 2
#define BITS 16
static const CLSID _CLSID_MMDeviceEnumerator = { 0xbcde0395, 0xe52f, 0x467c, { 0x8e,0x3d, 0xc4,0x57,0x92,0x91,0x69,0x2e } };
static const IID _IID_IMMDeviceEnumerator = { 0xa95664d2, 0x9614, 0x4f35, { 0xa7,0x46, 0xde,0x8d,0xb6,0x36,0x17,0xe6 } };
static const IID _IID_IAudioClient = { 0x1cb9ad4c, 0xdbfa, 0x4c32, { 0xb1,0x78, 0xc2,0xf5,0x68,0xa7,0x03,0xb2 } };
static const IID _IID_IAudioRenderClient = { 0xf294acfc, 0x3146, 0x4483, { 0xa7,0xbf, 0xad,0xdc,0xa7,0xc2,0x60,0xe2 } };
typedef struct {
	IAudioClient *cli;
	IAudioRenderClient *rdr;
	UINT32 bflen;
} mmdev_str;
void snd_mmdev_init(void **arg) {
	CoInitialize(NULL);
	mmdev_str *ctx = malloc(sizeof(mmdev_str));
	*arg = ctx;
	IMMDeviceEnumerator *enumer; IMMDevice *dev;
	WAVEFORMATEX wfx;
	CoCreateInstance(&_CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &_IID_IMMDeviceEnumerator, (void**)&enumer);
	enumer->lpVtbl->GetDefaultAudioEndpoint(enumer, eRender, eConsole, &dev);
	dev->lpVtbl->Activate(dev, &_IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&ctx->cli);
	dev->lpVtbl->Release(dev);
	enumer->lpVtbl->Release(enumer);
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = CHANNELS;
	wfx.nSamplesPerSec = RATE;
	wfx.wBitsPerSample = BITS;
	wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize = 0;
	// FIXME if (cli->lpVtbl->IsFormatSupported(cli, AUDCLNT_SHAREMODE_SHARED, &wfx, &wfxr) != S_OK)
	ctx->cli->lpVtbl->Initialize(ctx->cli, AUDCLNT_SHAREMODE_SHARED, 0, REFTIME(PERIOD_SIZE * PERIODS), 0, &wfx, NULL);
	ctx->cli->lpVtbl->GetBufferSize(ctx->cli, &ctx->bflen);
	ctx->cli->lpVtbl->GetService(ctx->cli, &_IID_IAudioRenderClient, (void**)&ctx->rdr);
	ctx->cli->lpVtbl->Start(ctx->cli);
}
int snd_mmdev_write(void *arg, void *buf, int len) {
	mmdev_str *ctx = arg;
	UINT32 pad;
	BYTE *ptr;
check:
	ctx->cli->lpVtbl->GetCurrentPadding(ctx->cli, &pad);
	pad = ctx->bflen - pad;
	if (pad < PERIOD_SIZE) { SLEEP_MS; goto check; }
	if (pad > len) pad = len;
	ctx->rdr->lpVtbl->GetBuffer(ctx->rdr, pad, &ptr);
	memcpy(ptr, buf, pad * CHANNELS * BITS / 8);
	ctx->rdr->lpVtbl->ReleaseBuffer(ctx->rdr, pad, 0);
	return pad;
}
void snd_mmdev_close(void *arg) {
	mmdev_str *ctx = arg;
	ctx->cli->lpVtbl->Stop(ctx->cli);
	ctx->rdr->lpVtbl->Release(ctx->rdr);
	ctx->cli->lpVtbl->Release(ctx->cli);
	free(ctx);
	CoUninitialize();
}
#endif
