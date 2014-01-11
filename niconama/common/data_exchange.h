#pragma once

#if _WIN32
#include <windows.h>
#define THREAD_TYPE HANDLE
#define THREAD_RET_TYPE DWORD
#define THREAD_CREATE_WIN(a, b, c) \
	c = CreateThread(NULL, 0, a, b, 0, NULL)
#define THREAD_CREATE_WIN2(a, b) \
	CreateThread(NULL, 0, a, b, 0, NULL)
#define THREAD_CREATE_POS(a, b, c) \
	c = _beginthreadex(NULL, 0, a, b, 0, NULL)
#define THREAD_CREATE_POS2(a, b) \
	_beginthreadex(NULL, 0, a, b, 0, NULL)
#define THREAD_WAIT(a) \
	WaitForSingleObject(a, INFINITE)
#define THREAD_RETURN_SUCCESS 0
#else
#include <pthread.h>
#define THREAD_TYPE pthread_t
#define THREAD_RET_TYPE *void
#define THREAD_CREATE_WIN(a, b, c) THREAD_CREATE_POS(a, b, c)
#define THREAD_CREATE_WIN2(a, b, NULL)
#define THREAD_CREATE_POS2(a, b, NULL)
#define THREAD_CREATE_POS(a, b, c) \
	pthread_create(&(c), NULL, a, b)
#define THREAD_WAIT(a) \
	pthread_join(&(a), NULL)
#define THREAD_RETURN_SUCCESS NULL
#endif

#include <stdint.h>
#include <librtmp/rtmp.h>

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int stop_flag;

#define rtmp_buf_video_len 64
#define rtmp_buf_audio_len 256

extern RTMPPacket **rtmp_buf_video;
extern RTMPPacket **rtmp_buf_audio;

typedef struct {
	int				(*init)		(void);
	int				(*header)	(RTMP*);
	THREAD_RET_TYPE	(*loop)		(void*);
	void			*user;
	char			*name;
} ENCODER;

#define audio_data_len 8
extern unsigned int samples_per_block;

typedef struct {
	float *data;//32bit float -1~1
	unsigned int *len;
} AUDIO_DATA;

extern AUDIO_DATA **audio_buf;
extern unsigned int audio_buf_len;

typedef struct {
	int				(*init)		(void);
	int				(*sync)		(void);
	THREAD_RET_TYPE (*loop)		(void*);
	void			*user;
	char			*name;
	char			*id;
} SOURCE;

#define INIT_AUDIO_DATA(ds, i) \
	ds.data=(float*)malloc(audio_data_len*samples_per_block*sizeof(float)); \
	memset(ds.data,0,audio_data_len*samples_per_block*sizeof(float)); \
	ds.len=(unsigned int*)malloc(audio_data_len*sizeof(unsigned int)); \
	for(i=0;i<audio_data_len;i++)ds.len[i]=samples_per_block

#ifdef __cplusplus
}
#endif
