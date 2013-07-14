//TODO リストビュー
//TODO タスクバー・メニューバー・ステータスバー
#define UNICODE
#define _UNICODE

#include <windows.h>
#include <commctrl.h>
#include <wchar.h>
#include <tchar.h>

#ifndef IDC_STATIC
	#define IDC_STATIC -1
#endif
#define IDC_SAVE 30011
#define IDC_EXIT 30012
#define IDC_OPEN 30013
#define IDC_RESET 30014
#define IDC_CLOSE 30015
#define IDC_VENDOR 30101
#define IDC_TITLE 30102
#define IDC_VERSION 30103
#define IDC_ALBUM 30104
#define IDC_TRACKNUMBER 30105
#define IDC_ARTIST 30106
#define IDC_PERFORMER 30107
#define IDC_COPYRIGHT 30108
#define IDC_LICENSE 30109
#define IDC_ORGANIZATION 30110
#define IDC_DESCRIPTION 30111
#define IDC_GENRE 30112
#define IDC_YEAR 30113
#define IDC_MONTH 30114
#define IDC_DAY 30115
#define IDC_HOUR 30116
#define IDC_MINUTE 30117
#define IDC_SECOND 30118
#define IDC_LOCATION 30119
#define IDC_CONTACT 30120
#define IDC_ISRC 30121
#define IDC_TOOLBAR 30122
#define IDC_LIST 30123
#define IDC_STATUS 30124

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LPCTSTR szClassName = TEXT("main");
LPCTSTR error = TEXT("Error");
LPCTSTR alloc_msg = TEXT("Cannot alloc memory.");
const TCHAR equal = L'=';

struct VORBIS_TAG {
	LPCTSTR n;
	DWORD i;
};

struct VORBIS_TAG vtags[] = {
	{TEXT("VENDOR"),IDC_VENDOR},
	{TEXT("TITLE"),IDC_TITLE},
	{TEXT("VERSION"),IDC_VERSION},
	{TEXT("ALBUM"),IDC_ALBUM},
	{TEXT("TRACKNUMBER"),IDC_TRACKNUMBER},
	{TEXT("ARTIST"),IDC_ARTIST},
	{TEXT("PERFORMER"),IDC_PERFORMER},
	{TEXT("COPYRIGHT"),IDC_COPYRIGHT},
	{TEXT("LICENSE"),IDC_LICENSE},
	{TEXT("ORGANIZATION"),IDC_ORGANIZATION},
	{TEXT("DESCRIPTION"),IDC_DESCRIPTION},
	{TEXT("GENRE"),IDC_GENRE},
	{TEXT("LOCATION"),IDC_LOCATION},
	{TEXT("CONTACT"),IDC_CONTACT},
	{TEXT("ISRC"),IDC_ISRC},
	{TEXT("DATE"),IDC_YEAR}
};

HANDLE hHeap,hList,hTB,hStatus;
HFONT hfont;

TCHAR filename[32768];
OPENFILENAME ofn;

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow) {
	MSG msg;
	BOOL bRet;
	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hCurInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if(!RegisterClassEx(&wc))
		return FALSE;
	
	HWND hWnd = CreateWindowEx(WS_EX_CONTROLPARENT,szClassName,TEXT("Flac Tag Editor"), WS_CAPTION|WS_OVERLAPPED|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_SYSMENU,CW_USEDEFAULT,CW_USEDEFAULT,
		CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hCurInst,NULL);
	if(!hWnd)
		return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	while((bRet = GetMessage(&msg,NULL,0,0))!=0) {
		if(bRet == -1)
			break;
		if(IsDialogMessage(hWnd,&msg))
			continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

BOOL SetValidFieldText(WCHAR * const name,HWND hWnd) {
	WCHAR *value = wcschr(name,equal);
	if (value == NULL)
		return FALSE;
	*value = 0;
	value++;
	//TODO
	return FALSE;
}

const BYTE flac[] = {'f','L','a','C'};

BOOL OpenFlacFile(HWND hWnd) {
	HANDLE fh;
	BYTE raw[4];
	unsigned int  size = 0;
	DWORD readed;
	LPSTR data;
	WCHAR * wdata;
	
	if ((fh = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL)) == INVALID_HANDLE_VALUE) {
		MessageBox(hWnd,TEXT("Cannot open flac file."),error,MB_OK|MB_ICONERROR);
		return FALSE;
	}
	ReadFile(fh,raw,4,&readed,NULL);
	if (raw[0]!=flac[0]||raw[1]!=flac[1]||raw[2]!=flac[2]||raw[3]!=flac[3]) {
		MessageBox(hWnd,TEXT("This is not flac file."),error,MB_OK|MB_ICONERROR);
		CloseHandle(fh);
		return FALSE;
	}
	
	hHeap = HeapCreate(0,1024,0);
	
	SetWindowText(hWnd,filename);
	SendMessage(hTB,TB_SETSTATE,IDC_RESET,TBSTATE_ENABLED);
	//TODO RESET EDIT
	ReadFile(fh,raw, 4, &readed,NULL);
	while(readed > 3) {
		if ((raw[0] & 127) == 4) {
			unsigned int  j, cont,bytelen;
			ReadFile(fh,&size,4,&readed,NULL);
			data = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,size+1);
			if(data == NULL) {
				MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
				HeapDestroy(hHeap);
				CloseHandle(fh);
				return FALSE;
			}
			ReadFile(fh,data,size,&readed,NULL);
			bytelen = MultiByteToWideChar(CP_UTF8,0,data,-1,NULL,0) + 1;
			wdata = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,bytelen*sizeof(WCHAR));
			if(wdata == NULL) {
				MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
				HeapDestroy(hHeap);
				CloseHandle(fh);
				return FALSE;
			}
			
			MultiByteToWideChar(CP_UTF8,0,data,-1,wdata,bytelen*sizeof(WCHAR));
			//SetWindowText(vtags[0].box,wdata);
			HeapFree(hHeap,0,data);
			ReadFile(fh,&cont,4,&readed,NULL);
			for(j = 0; j < cont; j++){
				ReadFile(fh,&size,4,&readed,NULL);
				data = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,size+1);
				if(data == NULL) {
					MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
					HeapDestroy(hHeap);
					CloseHandle(fh);
					return FALSE;
				}
				ReadFile(fh,data,size,&readed,NULL);
				bytelen = MultiByteToWideChar(CP_UTF8,0,data,-1,NULL,0) + 1;
				wdata = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,bytelen*sizeof(WCHAR));
				if(wdata == NULL) {
					MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
					HeapDestroy(hHeap);
					CloseHandle(fh);
					return FALSE;
				}
				MultiByteToWideChar(CP_UTF8,0,data,-1,wdata,bytelen*sizeof(WCHAR));
				SetValidFieldText(wdata,hWnd);
				HeapFree(hHeap,0,data);
			}
			break;
		}
		if (raw[0] & 128)
			break;
		size = (((unsigned int)raw[1]) << 16) |
			(((unsigned int)raw[2]) << 8) |
			((unsigned int)raw[3]);
		SetFilePointer(fh,size,NULL,FILE_CURRENT);
		ReadFile(fh,raw, 4, &readed,NULL);
	}
	HeapDestroy(hHeap);
	CloseHandle(fh);
	return TRUE;
}

BOOL SaveFlacFile(HWND hWnd) {
	hHeap = HeapCreate(0,1024,0);
	BYTE * raw;
	DWORD totallen = 0;
	{
		DWORD vendlen, size = 0;
		BYTE *vendor;
		WCHAR *wdata;
		LPSTR *data = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,sizeof(BYTE*));
		if(data == NULL) {
			MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
			HeapDestroy(hHeap);
			return FALSE;
		}
		/*{
			int i,c,cache,bytelen;
			if ((cache = GetWindowTextLength(vtags[0].box)) > 0) {
				wdata = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,sizeof(WCHAR)*(cache+1));
				GetWindowText(vtags[0].box,wdata,(cache+1)*sizeof(WCHAR));
				bytelen = WideCharToMultiByte(CP_UTF8,0,wdata,-1,NULL,0,NULL,NULL) + 1;
				vendor = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,bytelen);
				if(wdata == NULL || vendor == NULL) {
					MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
					HeapDestroy(hHeap);
					return FALSE;
				}
				WideCharToMultiByte(CP_UTF8,0,wdata,-1,vendor,cache*sizeof(WCHAR)+1,NULL,NULL);
				HeapFree(hHeap,0,wdata);
			} else {
				vendor = "reference libFLAC 1.2.1 20070917";
			}
			vendlen = strlen(vendor);
			
			totallen += 4+vendlen+4;
			for (i = 1,c = sizeof(vtags)/sizeof(vtags[0]);i<c;i++) {
				if ((cache = GetWindowTextLength(vtags[i].box)) > 0) {
					data = HeapReAlloc(hHeap,HEAP_ZERO_MEMORY,data,sizeof(BYTE*)*(size+1));
					wdata = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,sizeof(WCHAR)*(cache+wcslen(vtags[i].n)+2));
					if(data == NULL || wdata == NULL) {
						MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
						HeapDestroy(hHeap);
						return FALSE;
					}
					wcscpy(wdata,vtags[i].n);
					*(wdata+wcslen(wdata)) = equal;
					GetWindowText(vtags[i].box,wdata+wcslen(wdata),(cache+1)*sizeof(WCHAR));
					bytelen = WideCharToMultiByte(CP_UTF8,0,wdata,-1,NULL,0,NULL,NULL) + 1;
					data[size] = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,bytelen);
					if(data[size] == NULL) {
						MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
						HeapDestroy(hHeap);
						return FALSE;
					}
					WideCharToMultiByte(CP_UTF8,0,wdata,-1,data[size],bytelen,NULL,NULL);
					totallen += strlen(data[size]) + 4;
					HeapFree(hHeap,0,wdata);
					size++;
				}
			}
		}*/
		{
			int copyied = 0,i,tocopy;
			raw = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,totallen);
			if(raw == NULL) {
				MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
				HeapDestroy(hHeap);
				return FALSE;
			}
			memcpy(raw, &vendlen, 4);
			copyied += 4;
			
			memcpy(raw + copyied, vendor, vendlen);
			copyied += vendlen;
			HeapFree(hHeap,0,vendor);
			
			memcpy(raw + copyied, &size, 4);
			copyied += 4;
			
			for (i=0;i<size;i++) {
				tocopy = strlen(data[i]);
				
				memcpy(raw + copyied, &tocopy, 4);
				copyied += 4;
				
				memcpy(raw + copyied, data[i], tocopy);
				copyied += tocopy;
				
				HeapFree(hHeap,0,data[i]);
			}
			HeapFree(hHeap,0,data);
			if(copyied != totallen) {
				MessageBox(hWnd,TEXT("Size is invalid."),error,MB_OK|MB_ICONERROR);
				HeapDestroy(hHeap);
				return FALSE;
			}
		}
	}
	HANDLE rfh,wfh;
	BYTE rawread[4];
	BYTE *buf;
	BOOL commentdone = FALSE;
	DWORD donelen, donelenw;
	int size;
	LPTSTR cachefile = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,(_tcslen(filename)+7)*sizeof(TCHAR));
	if(cachefile == NULL) {
		MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
		HeapDestroy(hHeap);
		return FALSE;
	}
	_tcscpy(cachefile,filename);
	_tcscat(cachefile,TEXT(".cache"));
	rfh = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!rfh) {
		MessageBox(hWnd,TEXT("Cannot open flac file."),error,MB_ICONERROR|MB_OK);
		HeapDestroy(hHeap);
		return FALSE;
	}
	wfh = CreateFile(cachefile,GENERIC_WRITE,0,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!wfh) {
		MessageBox(hWnd,TEXT("Cannot open flac file."),error,MB_ICONERROR|MB_OK);
		CloseHandle(rfh);
		HeapDestroy(hHeap);
		return FALSE;
	}
	ReadFile(rfh,rawread,4,&donelen,NULL);
	if (rawread[0]!=flac[0]||rawread[1]!=flac[1]||rawread[2]!=flac[2]||rawread[3]!=flac[3]) {
		MessageBox(hWnd,TEXT("This is not flac file."),error,MB_OK|MB_ICONERROR);
		CloseHandle(rfh);
		CloseHandle(wfh);
		HeapDestroy(hHeap);
		return FALSE;
	}
	WriteFile(wfh,rawread,4,&donelen,NULL);
	ReadFile(rfh,rawread,4,&donelen,NULL);
	while(donelen > 3) {
		size = (((unsigned int)rawread[1]) << 16) |
			(((unsigned int)rawread[2]) << 8) |
			((unsigned int)rawread[3]);
		if ((rawread[0] & 127) == 4) {
			BYTE writedata[4];
			writedata[0] = (rawread[0] & 128) | 4;
			writedata[1] = (totallen >> 16) & 255;
			writedata[2] = (totallen >> 8) & 255;
			writedata[3] = totallen & 255;
			WriteFile(wfh,writedata,4,&donelen,NULL);
			WriteFile(wfh,raw,totallen,&donelen,NULL);
			commentdone = TRUE;
			SetFilePointer(rfh,size,NULL,FILE_CURRENT);
			ReadFile(rfh,rawread, 4, &donelen,NULL);
			continue;
		}
		if (rawread[0] & 128 && !commentdone) {
			BYTE writedata[4];
			if ((rawread[0] & 127) != 1) {
				rawread[0] = rawread[0] & 127;
				WriteFile(wfh,rawread,4,&donelen,NULL);
				buf = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,size);
				if(buf == NULL) {
					MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
					CloseHandle(rfh);
					CloseHandle(wfh);
					HeapDestroy(hHeap);
					return FALSE;
				}
				ReadFile(rfh,buf,size,&donelen,NULL);
				WriteFile(wfh,buf,donelen,&donelenw,NULL);
				HeapFree(hHeap,0,buf);
				
				writedata[0] = 132;
				writedata[1] = (totallen >> 16) & 255;
				writedata[2] = (totallen >> 8) & 255;
				writedata[3] = totallen & 255;
				WriteFile(wfh,writedata,4,&donelen,NULL);
				WriteFile(wfh,raw,totallen,&donelen,NULL);
				commentdone = TRUE;
				break;
			} else {
				writedata[0] = 4;
				writedata[1] = (totallen >> 16) & 255;
				writedata[2] = (totallen >> 8) & 255;
				writedata[3] = totallen & 255;
				WriteFile(wfh,writedata,4,&donelen,NULL);
				WriteFile(wfh,raw,totallen,&donelen,NULL);
				commentdone = TRUE;
			}
		}
		WriteFile(wfh,rawread,4,&donelen,NULL);
		buf = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,size);
		if(buf == NULL) {
			MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
			CloseHandle(rfh);
			CloseHandle(wfh);
			HeapDestroy(hHeap);
			return FALSE;
		}
		ReadFile(rfh,buf,size,&donelen,NULL);
		WriteFile(wfh,buf,donelen,&donelenw,NULL);
		HeapFree(hHeap,0,buf);
		if (rawread[0] & 128)
			break;
		ReadFile(rfh,rawread, 4, &donelen,NULL);
	}
	DWORD filesize;
	GetFileSize(rfh,&filesize);
	filesize -= SetFilePointer(rfh,0,NULL,FILE_CURRENT);
	filesize = (filesize / 10) + 1;
	buf = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,filesize);
	if (buf == NULL) {
		MessageBox(hWnd,alloc_msg,error,MB_OK|MB_ICONERROR);
		CloseHandle(rfh);
		CloseHandle(wfh);
		HeapDestroy(hHeap);
		return FALSE;
	}
	while(TRUE) {
		ReadFile(rfh,buf,filesize,&donelen,NULL);
		WriteFile(wfh,buf,donelen,&donelenw,NULL);
		if (donelen < filesize)
			break;
	}
	CloseHandle(rfh);
	CloseHandle(wfh);
	DeleteFile(filename);
	MoveFile(cachefile,filename);
	HeapDestroy(hHeap);
	return TRUE;
}

int isnumber(TCHAR* str){
	TCHAR ch;
	int rp = 0;
	do{
		ch = *(str + rp);
		rp += 1;
		if(_istdigit(ch) == 0)
			return 0;
	}while(*(str+rp) != '\0');
	return 1;
}

HANDLE hmonth,hday,hhour,hminute,hsecond;

TBBUTTON tbb[] = {
		{STD_FILENEW,IDC_CLOSE,TBSTATE_ENABLED,TBSTYLE_BUTTON},
		{STD_FILEOPEN,IDC_OPEN,TBSTATE_ENABLED,TBSTYLE_BUTTON},
		{STD_FILESAVE,IDC_SAVE,TBSTATE_ENABLED,TBSTYLE_BUTTON},
		{STD_UNDO,IDC_RESET,TBSTATE_INDETERMINATE,TBSTYLE_BUTTON},
		{STD_DELETE,IDC_EXIT,TBSTATE_ENABLED,TBSTYLE_BUTTON}};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	static RECT rect,elserect;
	switch(msg){
		case WM_CREATE:
			{
				LVCOLUMN column = {};
				TBADDBITMAP tb = {};
				int stdid,i;
				InitCommonControls();
				hTB = CreateWindowEx(0,TOOLBARCLASSNAME,NULL,WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS,0,0,0,0,hWnd,(HMENU)IDC_TOOLBAR,((LPCREATESTRUCT)(lp))->hInstance,NULL);
				SendMessage(hTB,TB_BUTTONSTRUCTSIZE,(WPARAM)sizeof(TBBUTTON),0);
				tb.hInst = HINST_COMMCTRL;
				tb.nID = IDB_STD_SMALL_COLOR;
				stdid = (int)SendMessage(hTB,TB_ADDBITMAP,0,(LPARAM)&tb);
				for(i=0;i<4;i++)
					tbb[i].iBitmap += stdid;
				SendMessage(hTB,TB_ADDBUTTONS,(WPARAM)5,(LPARAM)&tbb[0]);
				
				hStatus = CreateWindowEx(0,STATUSCLASSNAME,NULL,WS_CHILD|SBARS_SIZEGRIP|CCS_BOTTOM|WS_VISIBLE,0,0,0,0,hWnd,(HMENU)IDC_STATUS,((LPCREATESTRUCT)(lp))->hInstance,NULL);
				SendMessage(hStatus,SB_SIMPLE,0,0);
				
				GetClientRect(hWnd,&rect);
				GetWindowRect(hTB,&elserect);
				rect.top += elserect.bottom - elserect.top;
				rect.bottom -= elserect.bottom - elserect.top;
				GetWindowRect(hStatus,&elserect);
				rect.bottom -= elserect.bottom - elserect.top;
				hList = CreateWindowEx(0,WC_LISTVIEW,NULL,WS_CHILD|WS_VISIBLE|LVS_REPORT,rect.left,rect.top,rect.right,rect.bottom,hWnd,(HMENU)IDC_LIST,((LPCREATESTRUCT)(lp))->hInstance, NULL);
				column.mask = LVCF_TEXT|LVCF_WIDTH;
				column.pszText = TEXT("Key");
				column.cx = rect.right/4;
				SendMessage(hList,LVM_INSERTCOLUMN,0,(LPARAM)&column);
				column.pszText = TEXT("Value");
				column.cx = -1+rect.right*3/4;
				SendMessage(hList,LVM_INSERTCOLUMN,1,(LPARAM)&column);
				
				SendMessage(hStatus,SB_SETTEXT,0,(LPARAM)TEXT("Done"));
				return 0;
			}
		case WM_SIZE:
			SendMessage(hTB,msg,wp,lp);
			SendMessage(hStatus,msg,wp,lp);
			GetClientRect(hWnd,&rect);
			GetWindowRect(hTB,&elserect);
			rect.top += elserect.bottom - elserect.top;
			rect.bottom -= elserect.bottom - elserect.top;
			GetWindowRect(hStatus,&elserect);
			rect.bottom -= elserect.bottom - elserect.top;
			MoveWindow(hList,rect.left,rect.top,rect.right,rect.bottom,TRUE);
			return 0;
		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_EXIT:
					SendMessage(hWnd,WM_CLOSE,0,0);
					return 0;
				case IDC_OPEN:
					ZeroMemory(&ofn,sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFilter = TEXT("Flac file(*.flac)\0*.flac\0");
					ofn.lpstrFile = filename;
					ofn.nMaxFile = sizeof(filename);
					ofn.Flags = OFN_FILEMUSTEXIST;
					ofn.lpstrTitle = TEXT("Open flac file");
					ofn.lpstrDefExt = TEXT("flac");
					if(GetOpenFileName(&ofn))
						OpenFlacFile(hWnd);
					return 0;
				case IDC_RESET:
					OpenFlacFile(hWnd);
					return 0;
				case IDC_CLOSE:
					SetWindowText(hWnd,TEXT("Flac Tag Editor"));
					ZeroMemory(filename,sizeof(filename));
					SendMessage(hTB,TB_SETSTATE,IDC_RESET,TBSTATE_INDETERMINATE);
					return 0;
				case IDC_SAVE:
					ZeroMemory(&ofn,sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFilter = TEXT("Flac file(*.flac)\0*.flac\0");
					ofn.lpstrFile = filename;
					ofn.nMaxFile = sizeof(filename);
					ofn.Flags = OFN_OVERWRITEPROMPT;
					ofn.lpstrTitle = TEXT("Save flac file");
					ofn.lpstrDefExt = TEXT("flac");
					if(GetSaveFileName(&ofn))
						SaveFlacFile(hWnd);
					return 0;
			}
			break;
		case WM_NOTIFY: 
			switch (((LPNMHDR)lp)->code) {
				case TTN_GETDISPINFO:
				{
					LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lp;
					lpttt->hinst = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
					switch (lpttt->hdr.idFrom) {
						case IDC_EXIT: 
							lpttt->lpszText = TEXT("Exit Application"); 
							return 0;
						case IDC_OPEN: 
							lpttt->lpszText = TEXT("Open File"); 
							return 0;
						case IDC_RESET: 
							lpttt->lpszText = TEXT("Reset Value"); 
							return 0;
						case IDC_CLOSE:
							lpttt->lpszText = TEXT("Clear Value");
							return 0;
						case IDC_SAVE:
							lpttt->lpszText = TEXT("Save File");
							return 0;
					}
				}
				break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}