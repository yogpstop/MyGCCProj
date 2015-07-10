#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#define GETSTDINHANDLE(hstd) HANDLE hstd = GetStdHandle(STD_INPUT_HANDLE)
#define CATTR_T DWORD
#define GETCATTR(hstd, str) GetConsoleMode(hstd, &str)
#define SETCATTR(hstd, str) SetConsoleMode(hstd, str)
#define CAMASK(str) str &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)
#define FPURGE(stdh, hstd) FlushConsoleInputBuffer(hstd)
#define CHECK_IN (!_kbhit())
#else
#include <stdio_ext.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#define hstdin STDIN_FILENO
#define GETSTDINHANDLE(hstd)
#define CATTR_T struct termios
#define GETCATTR(hstd, str) tcgetattr(hstd, &str)
#define SETCATTR(hstd, str) tcsetattr(hstd, TCSANOW, &str)
#define CAMASK(str) str.c_lflag &= ~(ICANON | ECHO)
#define FPURGE(stdh, hstd) __fpurge(stdh)
#define CHECK_IN (poll(&fds, 1, 0) < 1)
#endif
#include "main.h"
void console_clear() {
#ifdef _WIN32
	HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = { 0, 0 };
	DWORD done, size;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hc, &csbi);
	size = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hc, ' ', size, coord, &done);
	FillConsoleOutputAttribute(hc, csbi.wAttributes, size, coord, &done);
	SetConsoleCursorPosition(hc, coord);
	//CloseHandle(hc);
#else
	fputs("\ec", stdout);
#endif
}
void console() {
	GETSTDINHANDLE(hstdin);
	CATTR_T str, bak;
	GETCATTR(hstdin, str);
	bak = str;
	CAMASK(str);
	SETCATTR(hstdin, str);
	FPURGE(stdin, hstdin);
	unsigned char cbuf;
#ifndef _WIN32
	struct pollfd fds;
	fds.fd = 0;
	fds.events = POLLIN;
#endif
	while (1) {
		if (CHECK_IN) {
			if(force_exit_signal) break;
			SLEEP_MS;
			continue;
		}
		if (1 > fread(&cbuf, 1, 1, stdin)) {
			force_exit_signal = 1;
			exit_signal = 1;
			break;
		}
		if (cbuf == 'D') {
			exit_signal = 1;
		} else if (cbuf == 'C') {
			force_exit_signal = 1;
			exit_signal = 1;
			break;
		}
	}
	SETCATTR(hstdin, bak);
	console_clear();
}
