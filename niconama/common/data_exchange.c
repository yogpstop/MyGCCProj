#include "data_exchange.h"

unsigned int stop_flag = 0;

RTMPPacket **rtmp_buf_video;
RTMPPacket **rtmp_buf_audio;

unsigned int samples_per_block;

AUDIO_DATA **audio_buf;
unsigned int audio_buf_len = 0;

VIDEO_DATA **video_buf;
unsigned int video_buf_len = 0;

