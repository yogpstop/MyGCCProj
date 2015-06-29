#include <windows.h>
#include <vfw.h>

typedef struct {
	PAVIFILE fp;
	PAVISTREAM sp;
} avi_str;
avi_str *avi_init(char*, void*, LONG);
void avi_frame(avi_str*, LONG, void*, LONG, DWORD);
void avi_end(avi_str*);
