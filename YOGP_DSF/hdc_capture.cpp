#include "YOGP_DSF.h"

void C_HDC::releaseImage() {
}

float* C_HDC::getImage() {
 		BitBlt(hcpt,0,0,bmih.biWidth,bmih.biHeight,hdc,ix,iy,SRCCOPY);
		unit_conv(a,(unsigned char *)bitmap,bmih.biSizeImage,true);
		if(bmih.biHeight==oh&&bmih.biWidth==ow)
			return a;
		for(int x=0;x<ow;x++)
			for(int y=0;y<oh;y++)
				nearest_neighbor(x,y,scale,b,a,bmih.biWidth,ow);
		return b;
}


C_HDC::C_HDC(int inw,int inh,int outw,int outh,int inx,int iny) {
		ix=inx;
		iy=iny;	
		ow=outw;
		oh=outh;
		hdc = GetDC(GetDesktopWindow());
		hcpt = CreateCompatibleDC(hdc);
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = inw;
		bmih.biHeight = inh;
		bmih.biPlanes = 1;
		bmih.biBitCount = 32;
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = DIBSIZE(bmih);
		bmpcpt = CreateDIBSection(hdc,(BITMAPINFO *)&bmih,DIB_RGB_COLORS,&bitmap,NULL,0);
		SelectObject(hcpt,bmpcpt);
		a=new float[bmih.biSizeImage];
		b=new float[ow*oh*4];
		scale=min((float)bmih.biWidth/ow,(float)bmih.biHeight/oh);
}

C_HDC::~C_HDC() {
		DeleteObject(bmpcpt);
		DeleteDC(hcpt);
		ReleaseDC(GetDesktopWindow(),hdc);
		delete [] a;
		delete [] b;
}