#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "rtmp.h"
#include "options.h"
#include "data_exchange.h"
#include "fdk-aac.h"
#include <fdk-aac/aacenc_lib.h>

typedef struct {
	HANDLE_AACENCODER handle;
	AACENC_InfoStruct encInfo;
	AACENC_BufDesc in_buf;
	AACENC_BufDesc out_buf;
	AACENC_InArgs in_args;
	AACENC_OutArgs out_args;
	int in_bufSizes;
	int out_bufSizes;
} FDK_AAC_USER;
static FDK_AAC_USER fdk_aac_user;

static int in_bufElSizes = 2, out_bufElSizes = 1,
	in_bufferIdentifiers = IN_AUDIO_DATA,
		out_bufferIdentifiers = OUT_BITSTREAM_DATA;

int fdk_aac_init(void) {
	if(AACENC_OK!=aacEncOpen(&fdk_aac_user.handle, FDK_AAC_PS ? 7 : FDK_AAC_SBR ? 3 : 1, 2)) return -1;
	if(AACENC_OK!=aacEncoder_SetParam(fdk_aac_user.handle, AACENC_TRANSMUX, 0)) return -2;
	if(AACENC_OK!=aacEncoder_SetParam(fdk_aac_user.handle, AACENC_AOT,
		FDK_AAC_PS ? 29 : FDK_AAC_SBR ? 5 : 2)) return -3;
	if(FDK_AAC_VBR) {
		if(AACENC_OK!=aacEncoder_SetParam(fdk_aac_user.handle, AACENC_BITRATEMODE, FDK_AAC_VBR)) return -4;
	} else {
		if(AACENC_OK!=aacEncoder_SetParam(fdk_aac_user.handle, AACENC_BITRATE, AUDIO_BITRATE)) return -5;
	}
	if(AACENC_OK!=aacEncoder_SetParam(fdk_aac_user.handle, AACENC_SAMPLERATE, AUDIO_SAMPLE_RATE)) return -6;
	if(AACENC_OK!=aacEncoder_SetParam(fdk_aac_user.handle, AACENC_CHANNELMODE, MODE_2)) return -7;
	if(AACENC_OK!=aacEncoder_SetParam(fdk_aac_user.handle, AACENC_AFTERBURNER, FDK_AAC_AFTERBURNER)) return -8;
	if(FDK_AAC_PS)
		if(AACENC_OK!=aacEncoder_SetParam(fdk_aac_user.handle, AACENC_SIGNALING_MODE, 1)) return -9;
	if(AACENC_OK!=aacEncEncode(fdk_aac_user.handle, NULL, NULL, NULL, NULL)) return -10;
	if(AACENC_OK!=aacEncInfo(fdk_aac_user.handle, &fdk_aac_user.encInfo)) return -11;

	fdk_aac_user.in_buf.numBufs = 1;
	fdk_aac_user.out_buf.numBufs = 1;
	fdk_aac_user.in_buf.bufSizes = &fdk_aac_user.in_bufSizes;
	fdk_aac_user.out_buf.bufSizes = &fdk_aac_user.out_bufSizes;
	fdk_aac_user.in_buf.bufElSizes = &in_bufElSizes;
	fdk_aac_user.out_buf.bufElSizes = &out_bufElSizes;
	fdk_aac_user.in_buf.bufferIdentifiers = &in_bufferIdentifiers;
	fdk_aac_user.out_buf.bufferIdentifiers = &out_bufferIdentifiers;
	fdk_aac_user.in_args.numAncBytes = 0;
	samples_per_block = fdk_aac_user.encInfo.frameLength * AUDIO_CHANNELS;
	return 0;
}

int fdk_aac_header(RTMP *r) {
	int ret;
	RTMP_GET_INT_BUFFER(r,fdk_aac_user.encInfo.confSize+2,0,
					RTMP_PACKET_SIZE_LARGE, RTMP_PACKET_TYPE_AUDIO);
	r->m_write.m_body[0] = 0xAF;
	r->m_write.m_body[1] = 0x00;
	memcpy(r->m_write.m_body+2, fdk_aac_user.encInfo.confBuf,
									fdk_aac_user.encInfo.confSize);
	RTMP_SET_INT_SIZE(r, fdk_aac_user.encInfo.confSize+2);
	RTMP_INT_SEND(r, ret);
	return ret;
}

#define FDK_AAC_ENCODE_LAST(out, outlen, ret) \
	fdk_aac_user.in_args.numInSamples = -1; \
	fdk_aac_user.in_bufSizes = 0; \
	fdk_aac_user.out_bufSizes = outlen; \
	fdk_aac_user.in_buf.bufs = NULL; \
	fdk_aac_user.out_buf.bufs = (void**)out; \
	ret=AACENC_OK!=aacEncEncode(fdk_aac_user.handle,&fdk_aac_user.in_buf,&fdk_aac_user.out_buf,&fdk_aac_user.in_args,&fdk_aac_user.out_args)

#define FDK_AAC_ENCODE(in,inlen,out,outlen, ret) \
	fdk_aac_user.in_args.numInSamples = (inlen) * sizeof(int16_t); \
	fdk_aac_user.in_bufSizes=(inlen)*sizeof(int16_t)*fdk_aac_user.encInfo.inputChannels; \
	fdk_aac_user.out_bufSizes = outlen; \
	fdk_aac_user.in_buf.bufs = (void**)in; \
	fdk_aac_user.out_buf.bufs = (void**)out; \
	ret=AACENC_OK!=aacEncEncode(fdk_aac_user.handle,&fdk_aac_user.in_buf,&fdk_aac_user.out_buf,&fdk_aac_user.in_args,&fdk_aac_user.out_args)

THREAD_RET_TYPE fdk_aac_loop(void* dummy) {
	unsigned int len_flag = 0, block_pos = 0, sample_pos = 0, stream_pos = 0;
	unsigned int out_pos = 0, ret;
	unsigned long total_samples = 0;
	RTMPPacket *pkt;
	float *buf = (float*)malloc(samples_per_block*sizeof(float));
	int16_t *buf2 = (int16_t*)malloc(samples_per_block*sizeof(int16_t));
	void *ptr;
    while (!stop_flag) {
		if(len_flag) sleep(samples_per_block*1000/
						AUDIO_SAMPLE_RATE/2/AUDIO_CHANNELS);
		for(len_flag = 0, stream_pos = 0;
				stream_pos < audio_buf_len; stream_pos++)
			len_flag |= audio_buf[stream_pos]->len[block_pos];
		if(len_flag) continue;
		for(sample_pos=0;sample_pos<samples_per_block;sample_pos++)
			buf[sample_pos] = audio_buf[0]->data
							[block_pos*samples_per_block+sample_pos];
		memset(audio_buf[0]->data+block_pos*samples_per_block,0,
				samples_per_block*sizeof(float));
		audio_buf[0]->len[block_pos] = samples_per_block;
		for(stream_pos=1;stream_pos<audio_buf_len;stream_pos++) {
			for(sample_pos=0;sample_pos<samples_per_block;sample_pos++)
				buf[sample_pos] += audio_buf[stream_pos]->
							data[block_pos*samples_per_block+sample_pos];
			memset(audio_buf[stream_pos]->data+block_pos*samples_per_block,0,
					samples_per_block*sizeof(float));
			audio_buf[stream_pos]->len[block_pos] = samples_per_block;
		}

		for(sample_pos=0;sample_pos<samples_per_block;sample_pos++) {
			if(buf[sample_pos]>1)buf2[sample_pos]=INT16_MAX;
			else if(buf[sample_pos]<-1)buf2[sample_pos]=INT16_MIN;
			else buf2[sample_pos] = buf[sample_pos] * 32767;
		}
		RTMP_GET_AAC_BUFFER(pkt, fdk_aac_user.encInfo.maxOutBufBytes, total_samples);
		ptr = pkt->m_body + 2;
		FDK_AAC_ENCODE(&buf2, samples_per_block / AUDIO_CHANNELS,
						&ptr, fdk_aac_user.encInfo.maxOutBufBytes, ret);
		if(ret) return ret;
		RTMP_SET_SIZE(pkt, fdk_aac_user.out_args.numOutBytes + 2);
		total_samples += fdk_aac_user.out_args.numInSamples;
		rtmp_buf_audio[out_pos] = pkt;
		out_pos++;
		if(out_pos >= rtmp_buf_audio_len) out_pos = 0;
		block_pos++;
		if(block_pos >= audio_data_len) block_pos=0;
    }
	RTMP_GET_AAC_BUFFER(pkt, fdk_aac_user.encInfo.maxOutBufBytes, total_samples);
	ptr = pkt->m_body + 2;
	FDK_AAC_ENCODE_LAST(&ptr, fdk_aac_user.encInfo.maxOutBufBytes, ret);
	RTMP_SET_SIZE(pkt, fdk_aac_user.out_args.numOutBytes + 2);
	rtmp_buf_audio[out_pos] = pkt;
	aacEncClose(&fdk_aac_user.handle);
	return 0;
}
