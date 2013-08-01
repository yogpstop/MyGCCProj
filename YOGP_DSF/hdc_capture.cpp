#include "YOGP_DSF.h"

void C_HDC::releaseImage() {
}

void C_HDC::getImage(unsigned char *out) {
	BitBlt(hcpt,0,0,bmih.biWidth,bmih.biHeight,hdc,4,23,SRCCOPY);
	unsigned int x,y,s=0,d=0;
	const unsigned int wda=bmih.biWidth*3;
	unsigned int r,g,b;
	for(y=0;y<360;y++) {
			for(x=0;x<640;x++) {
				r=bitmap[s];
				r+=bitmap[(s++)+wda];
				g=bitmap[s];
				g+=bitmap[(s++)+wda];
				b=bitmap[s];
				b+=bitmap[(s++)+wda];
				r+=bitmap[s];
				r+=bitmap[(s++)+wda];
				g+=bitmap[s];
				g+=bitmap[(s++)+wda];
				b+=bitmap[s];
				b+=bitmap[(s++)+wda];
				out[d++]=(unsigned char)(r/4);
				out[d++]=(unsigned char)(g/4);
				out[d++]=(unsigned char)(b/4);
			}
			s+=wda;
		}
	return;
}

C_HDC::C_HDC(){
		bmih.biWidth=1280;
		bmih.biHeight=720;
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biPlanes = 1;
		bmih.biBitCount = 24;
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = DIBSIZE(bmih);
		hdc = GetDC(GetDesktopWindow());
		hcpt = CreateCompatibleDC(hdc);
		bmpcpt = CreateDIBSection(hdc,(BITMAPINFO *)&bmih,DIB_RGB_COLORS,(VOID **)&bitmap,NULL,0);
		SelectObject(hcpt,bmpcpt);
	}

C_HDC::~C_HDC() {
	DeleteObject(bmpcpt);
	DeleteDC(hcpt);
	ReleaseDC(GetDesktopWindow(),hdc);
}