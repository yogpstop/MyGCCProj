#include <stdint.h>
#include <windows.h>

#define HDC_get(str) BitBlt(str->hcpt,str->top,str->left,str->width,str->height,str->hdc,0,0,SRCCOPY)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	HANDLE hWnd;
	int top;
	int left;
	int width;
	int height;

	uint8_t *bitmap;

	HDC hdc;
	HDC hcpt;
	HBITMAP bmpcpt;
	BITMAPINFOHEADER bmih;
} HDC_STR;

void HDC_init(HDC_STR *str);
void HDC_exit(HDC_STR *str);

#ifdef __cplusplus
}
#endif
