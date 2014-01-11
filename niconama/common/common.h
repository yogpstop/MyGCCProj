#define VIDEO_ID 0x1
#define AUDIO_ID 0x2
#define STREAMER_ID 0x3

#define ENCODER_ID 0x4
#define MIXER_ID 0x8
#define FILTER_ID 0xC
#define SOURCE_ID 0x10


#define VIDEO_ENCODER_ID (VIDEO_ID|ENCODER_ID)
#define VIDEO_MIXER_ID (VIDEO_ID|MIXER_ID)
#define VIDEO_FILTER_ID (VIDEO_ID|FILTER_ID)
#define VIDEO_SOURCE_ID (VIDEO_ID|SOURCE_ID)
#define AUDIO_ENCODER_ID (AUDIO_ID|ENCODER_ID)
#define AUDIO_MIXER_ID (AUDIO_ID|MIXER_ID)
#define AUDIO_FILTER_ID (AUDIO_ID|FILTER_ID)
#define AUDIO_SOURCE_ID (AUDIO_ID|SOURCE_ID)

typedef struct {
	int ID;
	char *name;
	int enabled;
	union {
		int src_len;
		int buf_len;
	};
	BASE_STRUCT *src;
	int dst_len;
	BASE_DTRUCT *dst;
	int *buf_2len;
	int **buf_3len;
	union {
		RTMPPacket **rbuf;
		uint8_t **bbuf;
		uint8_t ***bbbuf;
		float **fbuf;
		float ***ffbuf;
	}
	STREAMER_SETTING set;
	union {
		THREAD_RET_TYPE (*exec)(void*);
		int (*do)();
	}
	int (*init)(void);
	int (*sync)(void);
	int (*header)(RTMP*);
	THREAD_RET_TYPE (*wait)(void*);
	void *prv;
} DATA_STRUCT;

typedef struct {
	unsigned int data_c;
	DATA_STRUCT *data;
} GLOBAL;
