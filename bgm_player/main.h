#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
// OS dependency section
#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS Sleep(1)
#define THREAD_T HANDLE
#define THREAD_RAC DWORD WINAPI
#define MUTEX_T CRITICAL_SECTION
#define CREATE_THREAD(handle, start, arg) handle = CreateThread(NULL, 0, start, arg, 0, NULL)
#define CREATE_THREAD_RT(handle, start, arg, rt) CREATE_THREAD(handle, start, arg); SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST)
#define JOIN_THREAD(handle) WaitForSingleObject(handle, INFINITE)
#define MUTEX_INIT(m) InitializeCriticalSection(m)
#define MUTEX_LOCK(m) EnterCriticalSection(m)
#define MUTEX_UNLOCK(m) LeaveCriticalSection(m)
#define MUTEX_DESTROY(m) DeleteCriticalSection(m)
#define DIR_SEP '\\'
#define DIR_SEPS "\\"
#define PFZ "I"
#else
#include <time.h>
#include <pthread.h>
extern const struct timespec ms;
#define SLEEP_MS nanosleep(&ms, NULL)
#define THREAD_T pthread_t
#define THREAD_RAC void *
#define MUTEX_T pthread_mutex_t
#define CREATE_THREAD_RT(handle, start, arg, rt) pthread_create(&handle, rt, start, arg)
#define CREATE_THREAD(handle, start, arg) CREATE_THREAD_RT(handle, start, arg, NULL)
#define JOIN_THREAD(handle) pthread_join(handle, NULL)
#define MUTEX_INIT(m) pthread_mutex_init(m, NULL)
#define MUTEX_LOCK(m) pthread_mutex_lock(m)
#define MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
#define MUTEX_DESTROY(m) pthread_mutex_destroy(m)
#define DIR_SEP '/'
#define DIR_SEPS "/"
#define PFZ "z"
#endif
// struct section
typedef struct {
	char *n;
	size_t f;
	size_t t;
} data_format;
typedef struct {
	size_t period;
	size_t buf_max;
	MUTEX_T *mutex;
	void *buf;
	unsigned char *update;
	char *name;
} pcm;
typedef struct {
	pcm p;
	size_t cur_id;
	size_t cur_period;
	size_t from;
	size_t to;
	data_format *list;
} buf_str;
// global variables
extern int force_exit_signal, exit_signal;
// thread section
THREAD_RAC   play_thread(void*);
THREAD_RAC buffer_thread(void*);
// fsutil section
char *dirname(char*);
char *abspath(char*);
// file format section
int flv_per_tag(FILE*, void*, int (*func)(uint8_t, uint32_t, unsigned int, void*, void*));
void mp4_read(FILE*, buf_str*);
void flv1_read(FILE*, buf_str*);
void flac_read(FILE*, buf_str*);
void riff_read(FILE*, buf_str*);
void bytes2buf(void*, size_t, buf_str*);
// sound output section
#define snd__(func, ...) _snd__(SNDAPI, func, __VA_ARGS__)
#define _snd__(api, func, ...) __snd__(api, func, __VA_ARGS__)
#define __snd__(api, func, ...) snd_ ## api ## _ ## func(__VA_ARGS__)
void snd_mmdev_init(void**);
int snd_mmdev_write(void*, void*, int);
void snd_mmdev_close(void*);
void snd_alsa_init(void**);
int snd_alsa_write(void*, void*, int);
void snd_alsa_close(void*);
// listing section
data_format* listing(char**);
void list_shuffle(data_format*);
void list_full_remove(data_format*);
void read_cue(char *, size_t, size_t *, size_t *);
void console_clear();
void console();
