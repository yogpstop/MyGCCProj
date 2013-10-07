#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "rtmp.h"
#include "options.h"
#include "fdk-aac.h"
#include <fdk-aac/aacenc_lib.h>

static HANDLE_AACENCODER handle;
static AACENC_InfoStruct encInfo;
static AACENC_BufDesc in_buf;
static AACENC_BufDesc out_buf;
static AACENC_InArgs in_args;
static AACENC_OutArgs out_args;
static int in_bufSizes;
static int out_bufSizes;

static int in_bufElSizes = 2, out_bufElSizes = 1,
	in_bufferIdentifiers = IN_AUDIO_DATA,
		out_bufferIdentifiers = OUT_BITSTREAM_DATA;

int fdk_aac_init(void) {
	if(AACENC_OK != aacEncOpen
			(&handle, FDK_AAC_PS ? 7 : FDK_AAC_SBR ? 3 : 1, 2)) return -1;
	if(AACENC_OK != aacEncoder_SetParam
			(handle, AACENC_TRANSMUX, 0)) return -2;
	if(AACENC_OK != aacEncoder_SetParam(handle,
			AACENC_AOT, FDK_AAC_PS ? 29 : FDK_AAC_SBR ? 5 : 2)) return -3;
	if(FDK_AAC_VBR) {
		if(AACENC_OK != aacEncoder_SetParam
					(handle, AACENC_BITRATEMODE, FDK_AAC_VBR)) return -4;
	} else {
		if(AACENC_OK != aacEncoder_SetParam
					(handle, AACENC_BITRATE, AUDIO_BITRATE)) return -5;
	}
	if(AACENC_OK != aacEncoder_SetParam
			(handle, AACENC_SAMPLERATE, AUDIO_SAMPLE_RATE)) return -6;
	if(AACENC_OK != aacEncoder_SetParam
			(handle, AACENC_CHANNELMODE, MODE_2)) return -7;
	if(AACENC_OK != aacEncoder_SetParam
			(handle, AACENC_AFTERBURNER, FDK_AAC_AFTERBURNER)) return -8;
	if(FDK_AAC_PS)
		if(AACENC_OK != aacEncoder_SetParam
					(handle, AACENC_SIGNALING_MODE, 1)) return -9;
	if(AACENC_OK != aacEncEncode(handle, NULL, NULL, NULL, NULL)) return -10;
	if(AACENC_OK != aacEncInfo(handle, &encInfo)) return -11;

	in_buf.numBufs = 1;
	out_buf.numBufs = 1;
	in_buf.bufSizes = &in_bufSizes;
	out_buf.bufSizes = &out_bufSizes;
	in_buf.bufElSizes = &in_bufElSizes;
	out_buf.bufElSizes = &out_bufElSizes;
	in_buf.bufferIdentifiers = &in_bufferIdentifiers;
	out_buf.bufferIdentifiers = &out_bufferIdentifiers;
	in_args.numAncBytes = 0;
	samples_per_block = encInfo.frameLength * AUDIO_CHANNELS;
	return 0;
}

int fdk_aac_header(RTMP *r) {
	int ret;
	RTMP_GET_INT_BUFFER(r, encInfo.confSize + 2, 0, LARGE, AUDIO);
	r->m_write.m_body[0] = 0xAF;
	r->m_write.m_body[1] = 0x00;
	memcpy(r->m_write.m_body + 2, encInfo.confBuf, encInfo.confSize);
	RTMP_SET_INT_SIZE(r, encInfo.confSize + 2);
	RTMP_INT_SEND(r, ret);
	if(!ret) return -1;
	return 0;
}

#define FDK_AAC_ENCODE_LAST(out, outlen, ret) \
	in_args.numInSamples = -1; \
	in_bufSizes = 0; \
	out_bufSizes = outlen; \
	in_buf.bufs = NULL; \
	out_buf.bufs = (void**)out; \
	ret = aacEncEncode(handle, &in_buf, &out_buf, &in_args, &out_args)

#define FDK_AAC_ENCODE(in, inlen, out, outlen, ret) \
	in_args.numInSamples = (inlen) * sizeof(int16_t); \
	in_bufSizes = (inlen) * sizeof(int16_t) * encInfo.inputChannels; \
	out_bufSizes = outlen; \
	in_buf.bufs = (void**)in; \
	out_buf.bufs = (void**)out; \
	ret = aacEncEncode(handle, &in_buf, &out_buf, &in_args, &out_args)

THREAD_RET_TYPE fdk_aac_loop(void* dummy) {
	unsigned int len_flag = 0, block_pos = 0, sample_pos = 0,
							stream_pos = 0, out_pos = 0, ret;
	unsigned long total_samples = 0;
	void *ptr;
	RTMPPacket *pkt;
	float *buf = (float*)calloc(samples_per_block, sizeof(float));
	int16_t *buf2 = (int16_t*)calloc(samples_per_block, sizeof(int16_t));
	if(!buf || !buf2) stop_flag = 1;
    while (!stop_flag) {
		if(len_flag) usleep(samples_per_block * 1000000 /
						AUDIO_SAMPLE_RATE / 2 / AUDIO_CHANNELS);
		for(len_flag = 0, stream_pos = 0;
				stream_pos < audio_buf_len; stream_pos++)
			len_flag |= audio_buf[stream_pos]->len[block_pos];
		if(len_flag) continue;
		if(rtmp_buf_audio[out_pos]) break;
		for(sample_pos = 0; sample_pos < samples_per_block; sample_pos++)
			buf[sample_pos] = audio_buf[0]->data
							[block_pos * samples_per_block + sample_pos];
		memset(audio_buf[0]->data + block_pos * samples_per_block,
				0, samples_per_block * sizeof(float));
		audio_buf[0]->len[block_pos] = samples_per_block;
		for(stream_pos = 1; stream_pos < audio_buf_len; stream_pos++) {
			for(sample_pos = 0; sample_pos < samples_per_block; sample_pos++)
				buf[sample_pos] += audio_buf[stream_pos]-> data
								[block_pos * samples_per_block + sample_pos];
			memset(audio_buf[stream_pos]->data + block_pos * samples_per_block,
					0, samples_per_block * sizeof(float));
			audio_buf[stream_pos]->len[block_pos] = samples_per_block;
		}

		for(sample_pos = 0; sample_pos < samples_per_block; sample_pos++) {
			if(buf[sample_pos] > 1) buf2[sample_pos] = INT16_MAX;
			else if(buf[sample_pos] < -1) buf2[sample_pos] = INT16_MIN;
			else buf2[sample_pos] = buf[sample_pos] * 32767;
		}
		RTMP_GET_AAC_BUFFER(pkt, encInfo.maxOutBufBytes, total_samples);
		ptr = pkt->m_body + 2;
		FDK_AAC_ENCODE(&buf2, samples_per_block / AUDIO_CHANNELS,
						&ptr, encInfo.maxOutBufBytes, ret);
		if(ret!=AACENC_OK) break;
		RTMP_SET_SIZE(pkt, out_args.numOutBytes + 2);
		total_samples += out_args.numInSamples;
		rtmp_buf_audio[out_pos] = pkt;
		out_pos++;
		if(out_pos >= rtmp_buf_audio_len) out_pos = 0;
		block_pos++;
		if(block_pos >= audio_data_len) block_pos = 0;
    }
	if(!rtmp_buf_audio[out_pos]) {
		RTMP_GET_AAC_BUFFER(pkt, encInfo.maxOutBufBytes, total_samples);
		ptr = pkt->m_body + 2;
		FDK_AAC_ENCODE_LAST(&ptr, encInfo.maxOutBufBytes, ret);
		RTMP_SET_SIZE(pkt, out_args.numOutBytes + 2);
		rtmp_buf_audio[out_pos] = pkt;
	}
	stop_flag = 1;
	aacEncClose(&handle);
	return THREAD_RETURN_SUCCESS;
}
