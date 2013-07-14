#define UNICODE
#define _UNICODE

#include <windows.h>
#include <tchar.h>

const TCHAR szClassName[] = TEXT("example_win_gui");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow) {
	MSG msg;
	BOOL bRet;

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hCurInst;
	wc.hIcon = (HICON)LoadImage(NULL,IDI_APPLICATION,IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_SHARED);
	wc.hCursor = (HCURSOR)LoadImage(NULL,IDC_ARROW,IMAGE_CURSOR,0,0,LR_DEFAULTSIZE|LR_SHARED);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = szClassName;
	wc.hIconSm = (HICON)LoadImage(NULL,IDI_APPLICATION,IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_SHARED);
	if (!RegisterClassEx(&wc))
		return FALSE;
	
	HWND hWnd = CreateWindowEx(0,szClassName,TEXT("Title bar"),WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hCurInst,NULL);
	if (!hWnd)
		return FALSE;
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if(bRet == -1)
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}