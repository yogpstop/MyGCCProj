#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#define CHANNELS 2
#define BITS 16
THREAD_RAC play_thread(void *buf) {
	void *handle;
	snd__(init, &handle);
	size_t remain, done, cur_id = 0;
	void *ptr;
	pcm *str = buf;
	MUTEX_LOCK(str->mutex + cur_id);
	while(1) {
		remain = str->period;
		ptr = str->buf + cur_id * str->period * CHANNELS * BITS / 8;
		if (str->update[cur_id]) {
			console_clear();
#ifdef _WIN32
			SetConsoleTitleA(str->name);
#else
			fputs("\e]0;", stdout);
			fputs(str->name, stdout);
			fputs("\a", stdout);
#endif
			fputs("Now playing... ", stdout);
			fputs(str->name, stdout);
			fputc('\n', stdout);
			free(str->name); str->name = NULL;
			str->update[cur_id] = 0;
		}
		while(remain > 0) {
			if(exit_signal) break;//TODO
			done = snd__(write, handle, ptr, remain);
			if(done < 0) continue;
			ptr += done * CHANNELS * BITS / 8;
			remain -= done;
		}
		exit_signal = 0;//TODO
		if(force_exit_signal) break;
		cur_id++;
		cur_id = cur_id & str->buf_max;
		MUTEX_LOCK(str->mutex + cur_id);
		MUTEX_UNLOCK(str->mutex + ((cur_id - 1) & str->buf_max));
	}
	MUTEX_UNLOCK(str->mutex + cur_id);
	snd__(close, handle);
	return 0;
}
