#include <stdlib.h>
#include <stdint.h>
#include "vfw.h"
#include "main.h"
#include "dbg.h"

vfw_str *vfw_init() {
	vfw_str *r = malloc(sizeof(vfw_str));
	r->hic = ICOpen(ICTYPE_VIDEO, mmioFOURCC('U','L','R','G'), ICMODE_COMPRESS);
	DWORD res = ICCompressGetFormat(r->hic, NULL, NULL);
	r->bihi = malloc(res);
	r->biho = malloc(res);
	ZeroMemory(r->bihi, res);
	ZeroMemory(r->biho, res);
	r->bihi->biSize = sizeof(BITMAPINFOHEADER);
	r->bihi->biWidth = 1280;
	r->bihi->biHeight = 720;
	r->bihi->biPlanes = 1;
	r->bihi->biBitCount = 32;
	r->bihi->biCompression = BI_RGB;
	r->bihi->biSizeImage = r->bihi->biWidth * r->bihi->biHeight *
			r->bihi->biBitCount / 8;
	ICCompressGetFormat(r->hic, r->bihi, r->biho);
	ICCompressBegin(r->hic, r->bihi, r->biho);
	r->odat = malloc(max(r->biho->biSizeImage,
			ICCompressGetSize(r->hic, r->bihi, r->biho)));
	r->i = 0;
	r->a = avi_init("V:\\out.avi", r->biho, res);
	return r;
}
void vfw_frame(vfw_str *s, void *in) {
	DWORD flags;
	s->bihi->biWidth = *((uint16_t*)(in + sizeof(LARGE_INTEGER)));
	s->bihi->biHeight = *((uint16_t*)(in + sizeof(LARGE_INTEGER) + sizeof(uint16_t)));
	DBG_PERF(3, 4);
	ICCompress(s->hic, 0, s->biho, s->odat, s->bihi, in + GLC_HDR_SIZE, NULL, &flags, ++s->i, 0, 0, NULL, NULL);
	DBG_PERF(3, 5);
	avi_frame(s->a, s->i, s->odat, s->biho->biSizeImage, flags & AVIIF_KEYFRAME);
}
void vfw_end(vfw_str *s) {
	avi_end(s->a);
	ICCompressEnd(s->hic);
	ICClose(s->hic);
	free(s->bihi);
	free(s->biho);
	free(s->odat);
	free(s);
}
