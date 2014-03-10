#include <stdio_ext.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <poll.h>
#include <signal.h>
extern int exit_signal, force_exit_signal;
extern const struct timespec ms;
void console() {
	struct termios str, bak;
	tcgetattr(STDIN_FILENO, &str);
	bak = str;
	str.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &str);
	unsigned char cbuf;
	__fpurge(stdin);
	struct pollfd fds;
	int ret;
	fds.fd = 0;
	fds.events = POLLIN;
	while(1) {
		ret = poll(&fds, 1, 0);
		if(ret != 1) {
			if(force_exit_signal) break;
			nanosleep(&ms, NULL);
			continue;
		}
		fread(&cbuf, 1, 1, stdin);
		if(cbuf == 'D') {
			exit_signal = 1;
		} else if(cbuf == 'C') {
			force_exit_signal = 1;
			exit_signal = 1;
		}
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &bak);
	fputs("\ec",stdout);
}
