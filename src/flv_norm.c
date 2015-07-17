#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "flv_core.h"
typedef struct flv_tag {
	uint8_t type;
	uint32_t ts;
	unsigned int len;
	void *dat;
	struct flv_tag *prev;
	struct flv_tag *next;
} flv_tag;
static flv_tag *cur_tag = NULL;
static int callback(uint8_t type, uint32_t ts, unsigned int len, void *dat) {
	if (type != 8 && type != 9) { fprintf(stderr,"WARNING: invalid TagType %2X\n", type); return 0; }
	if (!len) { fprintf(stderr, "WARNING: invalid DataSize\n"); return 0; }
	flv_tag *new = malloc(sizeof(flv_tag));
	new->type = type; new->ts = ts; new->len = len; new->dat = dat;
	if (cur_tag) { cur_tag->next = new; new->prev = cur_tag; }
	else { new->prev = NULL; }
	cur_tag = new;
	return 1;
}
static uint32_t norm(char *srcn, char *dstn) {
	flv_per_tag(srcn, callback);
	FILE *fp = fopen(dstn, "wb");
	flv_write_header(fp, 5);//FIXME
	void *last_sh_audio = NULL, *last_sh_video = NULL;
	unsigned int last_sh_audio_len, last_sh_video_len;
	int need = 0;
	while (cur_tag->prev) cur_tag = cur_tag->prev;
	while (1) {
		need &= ~1;
		if (       (cur_tag->type == 8 && (((uint8_t*)cur_tag->dat)[0] & 0xF0) == 0xA0 && ((uint8_t*)cur_tag->dat)[1] == 0)
				|| (cur_tag->type == 9 && (((uint8_t*)cur_tag->dat)[0] & 0x0F) == 0x07 && ((uint8_t*)cur_tag->dat)[1] == 0)) {
			if (cur_tag->type == 8) {
				if (!last_sh_audio || last_sh_audio_len != cur_tag->len || memcmp(last_sh_audio, cur_tag->dat, last_sh_audio_len)) {
					fprintf(stderr, "New audio sequence header at %u\n", cur_tag->ts);
					last_sh_audio = cur_tag->dat;
					last_sh_audio_len = cur_tag->len;
					need |= 1;
				}
			} else if (cur_tag->type == 9) {
				if (!last_sh_video || last_sh_video_len != cur_tag->len || memcmp(last_sh_video, cur_tag->dat, last_sh_video_len)) {
					fprintf(stderr, "New video sequence header at %u\n", cur_tag->ts);
					last_sh_video = cur_tag->dat;
					last_sh_video_len = cur_tag->len;
					need |= 1;
				}
			}
			if (need & 1) flv_write_tag(fp, cur_tag->type, cur_tag->ts, cur_tag->len, cur_tag->dat);
		} else {
			if (!(need & 2) && cur_tag->type == 9 && (((uint8_t*)cur_tag->dat)[0] & 0x0F) == 0x07) {
				if (((uint8_t*)cur_tag->dat)[0] != 0x17) 
				     need |= 1;
				else need |= 2;
			}
			if (!(need & 1)) {
				if        (cur_tag->type == 9 && !(need & 4)) {
					fprintf(stderr, "First video tag at %u\n", cur_tag->ts);
					need |= 4;
				} else if (cur_tag->type == 8 && !(need & 8)) {
					fprintf(stderr, "First audio tag at %u\n", cur_tag->ts);
					need |= 8;
				}
				flv_write_tag(fp, cur_tag->type, cur_tag->ts, cur_tag->len, cur_tag->dat);
			}
		}
		if (!cur_tag->next) break;
		cur_tag = cur_tag->next;
	}
	fclose(fp);
	while (cur_tag->prev) cur_tag = cur_tag->prev;
	while (cur_tag->next) { free(cur_tag->dat); cur_tag = cur_tag->next; free(cur_tag->prev); }
	free(cur_tag->dat);
	last_sh_video_len = cur_tag->ts;
	free(cur_tag);
	cur_tag = NULL;
	return last_sh_video_len;
}
static char *get_tmp() {
	char tmp[MAX_PATH + 1];
	char *ret = malloc(MAX_PATH + 1);
	GetTempPathA(MAX_PATH + 1, tmp);
	GetTempFileNameA(tmp, "yog", 0, ret);
	return ret;
}
static void *get_res(char *n, char *t, LPDWORD len) {
	HRSRC hr = FindResourceA(NULL, n, t);
	*len = SizeofResource(NULL, hr);
	HGLOBAL hg = LoadResource(NULL, hr);
	return LockResource(hg);
}
static char *get_file(BOOL WINAPI (*GetFileNameA)(LPOPENFILENAMEA), const char *filter, const DWORD flag, char *title, const char *ext) {
	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.lpstrTitle = title;
	ofn.lpstrDefExt = ext;
	ofn.nMaxFile = MAX_PATH + 1;
	ofn.lpstrFile = malloc(ofn.nMaxFile);
	ofn.lpstrFile[0] = 0;
	ofn.lpstrFilter = filter;
	ofn.Flags = flag;
	GetFileNameA(&ofn);
	return ofn.lpstrFile;
}
int main() {
	// Init variables
	SetConsoleTitleA("変数初期化中");
	DWORD dw; HANDLE hf; void *b;
	char *in, *out, *log, *exe, *tmp;
	// Init filenames
	SetConsoleTitleA("ファイル選択中");
	in = get_file(GetOpenFileNameA, "FLV ファイル (*.flv)\0*.FLV\0", OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, "入力ファイルを選択してください", "flv");
	out = get_file(GetSaveFileNameA, "AVI ファイル (*.avi)\0*.AVI\0", OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, "出力ファイルを選択してください", "avi");
	log = ""; //get_file(GetSaveFileNameA, "テキストファイル (*.txt)\0*.TXT\0", OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, "ログファイルを選択してください(任意)", "txt");
	if (!*in || !*out) { MessageBoxA(NULL, "入出力ファイルを正しく選択してください！", "エラー", MB_OK | MB_ICONERROR); return 1; }
	exe = get_tmp(); tmp = get_tmp();
	// Normalize flv
	SetConsoleTitleA("FLV修復中");
	/*uint32_t max = */norm(in, tmp);
	// Init ffmpeg args
	SetConsoleTitleA("変換準備中");
	DeleteFile(out);
	char *cmd = malloc(strlen(exe) + strlen(tmp) + strlen(out) + 49);
	cmd[0] = '"';
	strcpy(cmd + 1, exe);
	strcat(cmd, "\" -i \"");
	strcat(cmd, tmp);
	strcat(cmd, "\" -vcodec libutvideo -acodec pcm_s16le \"");
	strcat(cmd, out);
	strcat(cmd, "\"");
	// Write ffmpeg.exe
	b = get_res(MAKEINTRESOURCE(184), MAKEINTRESOURCE(184), &dw);
	hf = CreateFileA(exe, GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hf, b, dw, &dw, NULL);
	CloseHandle(hf);
	// Init log file
	if (*log) {
		SECURITY_ATTRIBUTES sa;
		ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		hf = CreateFileA(log, GENERIC_WRITE, 0, &sa, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	} else hf = NULL;
	// Start ffmpeg
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	if (hf) {
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdOutput = hf;
		si.hStdError = hf;
	} else {
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	}
	SetConsoleTitleA("変換実行中");
	CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	// Clean up
	SetConsoleTitleA("終了作業中");
	if (hf) CloseHandle(hf);
	DeleteFileA(exe);
	DeleteFileA(tmp);
	free(cmd);
	free(exe);
	free(tmp);
	//free(log);
	free(out);
	free(in);
	MessageBoxA(NULL, "変換が完了しました", "情報", MB_OK | MB_ICONINFORMATION);
	return 0;
}
