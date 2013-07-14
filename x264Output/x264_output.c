//----------------------------------------------------------------------------------
//		x264簡易出力プラグイン  for AviUtl ver0.98以降
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>

#include "resource.h"
#include "output.h"


//---------------------------------------------------------------------
//		出力プラグイン構造体定義
//---------------------------------------------------------------------
OUTPUT_PLUGIN_TABLE output_plugin_table = {
	(int)NULL,											//	フラグ
	"x264,qaac簡易出力プラグイン",						//	プラグインの名前
	"MP4 (*.mp4)\0*.mp4\0",								//	出力ファイルのフィルタ
	"プログラミング学習用に制作したプラグイン",			//	プラグインの情報
	NULL,												//	DLL開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,												//	DLL終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	func_output,										//	出力時に呼ばれる関数へのポインタ
	func_config,										//	出力設定のダイアログを要求された時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	func_config_get,									//	出力設定データを取得する時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	func_config_set,									//	出力設定データを設定する時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
};


//---------------------------------------------------------------------
//		出力プラグイン構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C OUTPUT_PLUGIN_TABLE __declspec(dllexport) * __stdcall GetOutputPluginTable(void) {
	return &output_plugin_table;
}


//---------------------------------------------------------------------
//		出力プラグイン内部変数
//---------------------------------------------------------------------
typedef struct {
	TCHAR	x264exc[8192];
	TCHAR	x264arg[8192];
	TCHAR	qaacexc[8192];
	TCHAR	qaacarg[8192];
	TCHAR	mp4bexc[8192];
	TCHAR	mp4barg[8192];
} CONFIG;
static CONFIG config = {};


//---------------------------------------------------------------------
//		出力プラグイン出力関数
//---------------------------------------------------------------------
BOOL output_mix(OUTPUT_INFO *oip,HANDLE hStdout) {
	TCHAR arg[8192];
	
	// 引数追加
	snprintf(arg,sizeof(arg),"mp4box %s -add \"%s.264\" -add \"%s.m4a\" -new \"%s\"",config.mp4barg,oip->savefile,oip->savefile,oip->savefile);
	
	// パイプ処理準備
	STARTUPINFO si = {};
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = hStdout;
	si.hStdError = hStdout;
	
	PROCESS_INFORMATION pi = {};
	
	// 起動
	if(!CreateProcess(config.mp4bexc,arg,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi)){
		MessageBox(NULL,"ミキサーの起動に失敗しました","エラー",MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	
	WaitForSingleObject(pi.hProcess,INFINITE);

	return TRUE;
}

BOOL output_audio(OUTPUT_INFO *oip,HANDLE hStdout,SECURITY_ATTRIBUTES *sa) {
	int		i,readed;
	void	*sample;
	TCHAR	arg[8192];
	DWORD	written;
	
	// 引数追加
	snprintf(arg,sizeof(arg),"qaac %s -R --raw-channels %d --raw-rate %d --raw-format S16L -o \"%s.m4a\" -",config.qaacarg,oip->audio_ch,oip->audio_rate,oip->savefile);
	
	// パイプ処理準備
	HANDLE readPipe,writePipe;
	
	if (!CreatePipe(&readPipe,&writePipe,sa,0) || !SetHandleInformation(writePipe, HANDLE_FLAG_INHERIT, 0)){
		MessageBox(NULL,"パイプの作成が出来ませんでした","エラー",MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	
	STARTUPINFO si = {};
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = readPipe;
	si.hStdOutput = hStdout;
	si.hStdError = hStdout;
	
	PROCESS_INFORMATION pi = {};
	
	// 起動
	if(!CreateProcess(config.qaacexc,arg,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi)){
		MessageBox(NULL,"音声エンコーダーの起動に失敗しました","エラー",MB_OK|MB_ICONSTOP);
		CloseHandle(readPipe);
		CloseHandle(writePipe);
		return FALSE;
	}
	
	CloseHandle(readPipe);
	
	WaitForInputIdle(pi.hProcess,INFINITE);
	
	// メインループ
	for(i=0;i<oip->audio_n;i+=oip->audio_rate){
		if( oip->func_is_abort() ) break;
		oip->func_rest_time_disp(i,oip->audio_n);
		sample = oip->func_get_audio(i,oip->audio_rate,&readed);
		WriteFile(writePipe,sample,oip->audio_size * readed,&written,0);
		oip->func_update_preview();
	}
	
	CloseHandle(writePipe);
	
	WaitForSingleObject(pi.hProcess,INFINITE);
	
	return TRUE;
}

BOOL output_video(OUTPUT_INFO *oip,HANDLE hStdout,SECURITY_ATTRIBUTES *sa) {
	int		i;
	void	*frame;
	TCHAR	arg[8192];
	DWORD	written;
	
	// 引数追加
	snprintf(arg,sizeof(arg),"x264 %s --input-res %dx%d --fps %d/%d --frames %d --input-csp bgr --demuxer raw --muxer raw --input-depth 8 -o \"%s.264\" -",config.x264arg,oip->w,oip->h,oip->rate,oip->scale,oip->n,oip->savefile);
	
	// パイプ処理準備
	HANDLE readPipe,writePipe;
	
	if (!CreatePipe(&readPipe,&writePipe,sa,0) || !SetHandleInformation(writePipe, HANDLE_FLAG_INHERIT, 0)){
		MessageBox(NULL,"パイプの作成が出来ませんでした","エラー",MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	
	STARTUPINFO si = {};
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = readPipe;
	si.hStdOutput = hStdout;
	si.hStdError = hStdout;
	
	PROCESS_INFORMATION pi = {};
	
	// 起動
	if(!CreateProcess(config.x264exc,arg,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi)){
		MessageBox(NULL,"映像エンコーダーの起動に失敗しました","エラー",MB_OK|MB_ICONSTOP);
		CloseHandle(readPipe);
		CloseHandle(writePipe);
		return FALSE;
	}
	
	CloseHandle(readPipe);
	
	WaitForInputIdle(pi.hProcess,INFINITE);
	
	// メインループ
	const int step = oip->w * 3;
	const int fstep = (oip->w * 3 + 3) & ~3;
	int y;
	for(i=0;i<oip->n;i++) {
		if( oip->func_is_abort() ) break;
		oip->func_rest_time_disp(i,oip->n);
		frame = oip->func_get_video(i);
		for (y = oip->h - 1; y >= -1; y--)
			WriteFile(writePipe,frame + y * fstep,step,&written,0);
		oip->func_update_preview();
	}
	
	CloseHandle(writePipe);
	
	WaitForSingleObject(pi.hProcess,INFINITE);
	
	return TRUE;
}

BOOL output(OUTPUT_INFO *oip, SECURITY_ATTRIBUTES *sa,HANDLE hStdout,BOOL mix) {
	if ((oip->flag & OUTPUT_INFO_FLAG_VIDEO) && output_video(oip, hStdout, sa) != TRUE) {
		MessageBox(NULL, "映像エンコードに失敗しました", "エラー", MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	if ((oip->flag & OUTPUT_INFO_FLAG_AUDIO) && output_audio(oip, hStdout, sa) != TRUE) {
		MessageBox(NULL, "音声エンコードに失敗しました", "エラー", MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	if (mix && output_mix(oip, hStdout) != TRUE) {
		MessageBox(NULL, "ミキシングに失敗しました", "エラー", MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	MessageBox(NULL, "出力に成功しました", "成功", MB_OK|MB_ICONINFORMATION);
	return TRUE;
}

BOOL func_output(OUTPUT_INFO *oip) {
	TCHAR	target[8192];
	SECURITY_ATTRIBUTES sa = {};
	BOOL mix = (oip->flag & (OUTPUT_INFO_FLAG_VIDEO | OUTPUT_INFO_FLAG_AUDIO)) == (OUTPUT_INFO_FLAG_VIDEO | OUTPUT_INFO_FLAG_AUDIO);
	sa.bInheritHandle = TRUE;
	sa.nLength = sizeof(sa);
	HANDLE hStdout = CreateFile("stdout.log",GENERIC_WRITE,FILE_SHARE_READ,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hStdout == INVALID_HANDLE_VALUE) {
		MessageBox(NULL,"標準出力・エラー出力のパイプ処理に失敗しました","エラー",MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	output(oip,&sa,hStdout,mix);
	if (mix){
		snprintf(target,sizeof(target),"%s.264",oip->savefile);
		DeleteFile(target);
		snprintf(target,sizeof(target),"%s.m4a",oip->savefile);
		DeleteFile(target);
	}
	CloseHandle(hStdout);
	return TRUE;
}


//---------------------------------------------------------------------
//		出力プラグイン設定関数
//---------------------------------------------------------------------

OPENFILENAME ofn;
TCHAR filename[4096];

LRESULT CALLBACK func_config_proc( HWND hdlg,UINT umsg,WPARAM wparam, LPARAM lparam )
{
	switch( umsg ) {
		case WM_INITDIALOG:
			SetDlgItemText(hdlg,IDC_EDIT1,config.x264exc);
			SetDlgItemText(hdlg,IDC_EDIT2,config.x264arg);
			SetDlgItemText(hdlg,IDC_EDIT3,config.qaacexc);
			SetDlgItemText(hdlg,IDC_EDIT4,config.qaacarg);
			SetDlgItemText(hdlg,IDC_EDIT5,config.mp4bexc);
			SetDlgItemText(hdlg,IDC_EDIT6,config.mp4barg);
			return TRUE;
		case WM_COMMAND:
			switch( LOWORD(wparam) ) {
				case IDCANCEL:
					EndDialog(hdlg,LOWORD(wparam));
					break;
				case IDOK:
					GetDlgItemText(hdlg,IDC_EDIT1,config.x264exc,sizeof(config.x264exc));
					GetDlgItemText(hdlg,IDC_EDIT2,config.x264arg,sizeof(config.x264arg));
					GetDlgItemText(hdlg,IDC_EDIT3,config.qaacexc,sizeof(config.qaacexc));
					GetDlgItemText(hdlg,IDC_EDIT4,config.qaacarg,sizeof(config.qaacarg));
					GetDlgItemText(hdlg,IDC_EDIT5,config.mp4bexc,sizeof(config.mp4bexc));
					GetDlgItemText(hdlg,IDC_EDIT6,config.mp4barg,sizeof(config.mp4barg));
					EndDialog(hdlg,LOWORD(wparam));
					break;
				case IDC_BUTTON1:
					{
						ZeroMemory(&ofn,sizeof(ofn));
						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = hdlg;
						ofn.lpstrFilter = "実行可能ファイル(*.exe)\0*.exe\0";
						ofn.lpstrFile = filename;
						ofn.nMaxFile = sizeof(filename);
						ofn.Flags = OFN_FILEMUSTEXIST;
						ofn.lpstrTitle = "x264の実行ファイルを選択してください";
						ofn.lpstrDefExt = ".exe";
						if(GetOpenFileName(&ofn))
							SetDlgItemText(hdlg,IDC_EDIT1,filename);
					}
					break;
				case IDC_BUTTON2:
					{
						ZeroMemory(&ofn,sizeof(ofn));
						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = hdlg;
						ofn.lpstrFilter = "実行可能ファイル(*.exe)\0*.exe\0";
						ofn.lpstrFile = filename;
						ofn.nMaxFile = sizeof(filename);
						ofn.Flags = OFN_FILEMUSTEXIST;
						ofn.lpstrTitle = "qaacの実行ファイルを選択してください";
						ofn.lpstrDefExt = ".exe";
						if(GetOpenFileName(&ofn))
							SetDlgItemText(hdlg,IDC_EDIT3,filename);
					}
					break;
				case IDC_BUTTON3:
					{
						ZeroMemory(&ofn,sizeof(ofn));
						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = hdlg;
						ofn.lpstrFilter = "実行可能ファイル(*.exe)\0*.exe\0";
						ofn.lpstrFile = filename;
						ofn.nMaxFile = sizeof(filename);
						ofn.Flags = OFN_FILEMUSTEXIST;
						ofn.lpstrTitle = "MP4BOXの実行ファイルを選択してください";
						ofn.lpstrDefExt = ".exe";
						if(GetOpenFileName(&ofn))
							SetDlgItemText(hdlg,IDC_EDIT5,filename);
					}
					break;
			}
			break;
	}
	return FALSE;
}

BOOL func_config( HWND hwnd,HINSTANCE dll_hinst )
{
	DialogBox(dll_hinst,MAKEINTRESOURCE(_CONFIG),hwnd,(DLGPROC)func_config_proc);
	return TRUE;
}

int func_config_get( void *data,int size )
{
	if( data ) memcpy(data,&config,sizeof(config));
	return sizeof(config);
}

int func_config_set( void *data,int size )
{
	if( size != sizeof(config) ) return (int) NULL;
	memcpy(&config,data,size);
	return size;
}


