#include "avi.h"
#include "main.h"

avi_str *avi_init(char *out, void *bih, LONG bihl) {
	avi_str *r = malloc(sizeof(avi_str));
	AVIFileInit();
	AVIFileOpenA(&r->fp, out, OF_CREATE | OF_WRITE, NULL);
	AVISTREAMINFO si;
	ZeroMemory(&si, sizeof(AVISTREAMINFO));
	si.fccType = streamtypeVIDEO;
	si.fccHandler = mmioFOURCC('U', 'L', 'R', 'G');
	si.dwScale = 1;
	si.dwRate = GLC_FPS;
	si.dwQuality = -1;
	SetRect(&si.rcFrame, 0, 0, 1280, 720);
	AVIFileCreateStream(r->fp, &r->sp, &si);
	AVIStreamSetFormat(r->sp, 0, bih, bihl);
	return r;
}
void avi_frame(avi_str *s, LONG n, void *dat, LONG dlen, DWORD flags) {
	AVIStreamWrite(s->sp, n, 1, dat, dlen, flags, NULL, NULL);
}
void avi_end(avi_str *s) {
	AVIStreamRelease(s->sp);
	AVIFileRelease(s->fp);
	AVIFileExit();
	free(s);
}
