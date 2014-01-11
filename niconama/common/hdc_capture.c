#include "hdc_capture.h"

void HDC_init(HDC_STR *str) {
	str->bmih.biWidth = str->width;
	str->bmih.biHeight = str->height;
	str->bmih.biSize = sizeof(BITMAPINFOHEADER);
	str->bmih.biPlanes = 1;
	str->bmih.biBitCount = 24;
	str->bmih.biCompression = BI_RGB;
	str->bmih.biSizeImage = DIBSIZE(str->bmih);
	str->hdc = GetDC(str->hWnd);
	str->hcpt = CreateCompatibleDC(str->hdc);
	str->bmpcpt = CreateDIBSection(str->hdc,(BITMAPINFO *)&str->bmih,
					DIB_RGB_COLORS,(VOID **)&str->bitmap,NULL,0);
	SelectObject(str->hcpt,str->bmpcpt);
}

void HDC_exit(HDC_STR *str) {
	DeleteObject(str->bmpcpt);
	DeleteDC(str->hcpt);
	ReleaseDC(str->hWnd,str->hdc);
}