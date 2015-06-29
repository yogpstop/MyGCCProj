#include <windows.h>
#include <vfw.h>
#include "avi.h"

typedef struct {
	HIC hic;
	LPBITMAPINFOHEADER bihi;
	LPBITMAPINFOHEADER biho;
	BYTE *odat;
	LONG i;
	avi_str *a;
} vfw_str;
vfw_str *vfw_init();
void vfw_frame(vfw_str *s, void *in);
void vfw_end(vfw_str *s);
