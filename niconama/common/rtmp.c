#include <unistd.h>
#include <malloc.h>
#include <stdint.h>
#include "xml_struct.h"
#include "rtmp.h"
#include "options.h"

#define RTMP_SLEEP_TIME 10000

#define RTMP_GET_INFO_INT_BUFFER(r, size, time, ptr) \
		RTMP_GET_INT_BUFFER(r, size + 8, time, LARGE, INFO); \
		ptr=((uint8_t*)r->m_write.m_body);SETDATAFRAME_WRITE(ptr)

static RTMP *r = NULL;

int rtmp_write_header(int(*video)(RTMP*),int(*audio)(RTMP*)) {
	double cac = 0;
	uint8_t *ptr;
	int ret;
	RTMP_GET_INFO_INT_BUFFER(r, 184, 0, ptr);

	ONMETADATA_WRITE(ptr); WRITE_4(ptr,0x07000000); ptr+=4;

	DURATION_WRITE(ptr,cac);
	FILESIZE_WRITE(ptr,cac);
	cac=10; AUDIOCODECID_WRITE(ptr,cac);
	cac=AUDIO_BITRATE; AUDIODATARATE_WRITE(ptr, cac);
	cac=AUDIO_SAMPLE_RATE; AUDIOSAMPLERATE_WRITE(ptr,cac);
	cac=16; AUDIOSAMPLESIZE_WRITE(ptr,cac);
	STEREO_WRITE(ptr,1);
	ENDOFECMA_WRITE(ptr);
	RTMP_SET_INT_SIZE(r, ptr-((uint8_t*)r->m_write.m_body));
	RTMP_INT_SEND(r, ret);
	if(!ret) return -3;
	if(video) if(video(r)) return -1;
	if(audio) if(audio(r)) return -2;
	return 0;
}

THREAD_RET_TYPE rtmp_loop(void*dummy) {
	unsigned int flag = 0, a_pos = 0, v_pos = 0, ret;
	if(!AUDIO_ENABLE && !VIDEO_ENABLE) goto exit;
	while (!stop_flag) {
		if (flag) usleep(RTMP_SLEEP_TIME);
		flag = (!rtmp_buf_video[v_pos] && VIDEO_ENABLE)
			|| (!rtmp_buf_audio[a_pos] && AUDIO_ENABLE);
		if (flag) continue;
		if (!VIDEO_ENABLE || (AUDIO_ENABLE && 
						rtmp_buf_video[v_pos]->m_nTimeStamp >
								rtmp_buf_audio[a_pos]->m_nTimeStamp)) {
			RTMP_SEND(r, rtmp_buf_audio[a_pos], ret);
			a_pos++;
			if (a_pos>=rtmp_buf_audio_len) a_pos=0;
			if (!ret) break;
		} else {
			RTMP_SEND(r, rtmp_buf_video[v_pos], ret);
			v_pos++;
			if (v_pos>=rtmp_buf_video_len) v_pos=0;
			if (!ret) break;
		}
	}
exit:
	stop_flag = 1;
	RTMP_Close(r);
	free(r);
	r = NULL;
	free(rtmp_buf_video);
	free(rtmp_buf_audio);
	return THREAD_RETURN_SUCCESS;
}

int rtmp_init(void) {
	char *url, *ptr;
	unsigned int i = 0;
	struct getpublishstatus *gps;
	if(r) return -255;
	{
		gps = getpublishstatus();
		if(!gps || !gps->rtmp_url || !gps->rtmp_ticket || !gps->rtmp_stream)
			return -1;
		url = (char*) malloc(strlen(gps->rtmp_url) +
					strlen(gps->rtmp_ticket) + strlen(gps->rtmp_stream) + 3);
		if(!url) return -2;
		ptr = gps->rtmp_url;
		while(1) { if(!*ptr) break; url[i] = *ptr; ptr++; i++; }
		ptr=gps->rtmp_ticket; url[i] = '?'; i++;
		while(1) { if(!*ptr) break; url[i] = *ptr; ptr++; i++; }
		ptr=gps->rtmp_stream; url[i] = '/'; i++;
		while(1) { if(!*ptr) break; url[i] = *ptr; ptr++; i++; }
		url[i] = 0;
		freegetpublishstatus(gps);
	}
	rtmp_buf_video = (RTMPPacket**) malloc
			(rtmp_buf_video_len * sizeof(RTMPPacket*));
	if(!rtmp_buf_video) return -3;
	rtmp_buf_audio = (RTMPPacket**) malloc
			(rtmp_buf_audio_len * sizeof(RTMPPacket*));
	if(!rtmp_buf_audio) return -4;
	r = calloc(1, sizeof(RTMP));
	if(!r) return -5;
	RTMP_Init(r); //void
	i = RTMP_SetupURL(r,url);
	if(!i) return -6;
	r->Link.protocol |= RTMP_FEATURE_WRITE;
	i = RTMP_Connect(r,NULL);
	if(!i) return -7;
	i = RTMP_ConnectStream(r,0);
	if(!i) return -8;
	free(url);
	return 0;
}
