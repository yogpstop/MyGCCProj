#include "YOGP_DSF.h"

#define ID_CLIENT 55
#define ID_TIMER 56
#define ID_WATCH 57
#define ID_TITLE 58
#define ID_LAYER 59
#define ID_START 60
#define ID_IMAGE 61

const TCHAR szClassName[] = TEXT("yogp_dsf");
HWND CLIENT,TIMER,WATCH,TITLE,LAYER,START,IMAGE;

extern DWORD yogpdsf_curmode;
extern FILETIME starttime;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
		case WM_COMMAND:
			switch(LOWORD(wp)){
				SWITCH(CLIENT);
				SWITCH(TIMER);
				SWITCH(WATCH);
				SWITCH(TITLE);
				SWITCH(LAYER);
				SWITCH(IMAGE);
				case ID_START: 
					{ 
						HANDLE hmut = OpenMutex(MUTEX_ALL_ACCESS,FALSE,MUTEX_WORD);
						yogpdsf_curmode ^= FLAG_START;
						SET_TEXT(START);
						SYSTEMTIME st;
						if(yogpdsf_curmode & FLAG_START) {
							GetLocalTime(&st);
							SystemTimeToFileTime(&st,&starttime);
						} else {
							ULONGLONG ull;
							FILETIME ft;
							GetLocalTime(&st);
							SystemTimeToFileTime(&st,&ft);
							ull = (((ULONGLONG) ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
							ull -= (((ULONGLONG) starttime.dwHighDateTime) << 32) + starttime.dwLowDateTime;
							starttime.dwLowDateTime  = (DWORD) (ull & 0xFFFFFFFF );
							starttime.dwHighDateTime = (DWORD) (ull >> 32 );
						}
						ReleaseMutex(hmut);
						CloseHandle(hmut);
						return 0;
					}
			}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_CREATE:
			{
				MY_CREATE_BUTTON(CLIENT,10,10);
				MY_CREATE_BUTTON(TIMER,10,40);
				MY_CREATE_BUTTON(WATCH,10,70);
				MY_CREATE_BUTTON(TITLE,10,100);
				MY_CREATE_BUTTON(LAYER,10,130);
				MY_CREATE_BUTTON(START,10,160);
				MY_CREATE_BUTTON(IMAGE,10,190);
				HANDLE hmut = OpenMutex(MUTEX_ALL_ACCESS,FALSE,MUTEX_WORD);
				SET_TEXT(CLIENT);
				SET_TEXT(TIMER);
				SET_TEXT(WATCH);
				SET_TEXT(TITLE);
				SET_TEXT(LAYER);
				SET_TEXT(START);
				SET_TEXT(IMAGE);
				ReleaseMutex(hmut);
				CloseHandle(hmut);
				return 0;
			}
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

int dsfCreateWindow() {
	MSG msg;
	BOOL bRet;

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = (HICON)LoadImage(NULL,IDI_APPLICATION,IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_SHARED);
	wc.hCursor = (HCURSOR)LoadImage(NULL,IDC_ARROW,IMAGE_CURSOR,0,0,LR_DEFAULTSIZE|LR_SHARED);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = szClassName;
	wc.hIconSm = (HICON)LoadImage(NULL,IDI_APPLICATION,IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_SHARED);
	if (!RegisterClassEx(&wc))
		return FALSE;
	
	HWND hWnd = CreateWindowEx(0,szClassName,TEXT("Title bar"),WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
		CW_USEDEFAULT,CW_USEDEFAULT,200,300,NULL,NULL,NULL,NULL);
	if (!hWnd)
		return FALSE;
	ShowWindow(hWnd,SW_SHOWMINNOACTIVE);
	UpdateWindow(hWnd);
	
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if(bRet == -1)
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}