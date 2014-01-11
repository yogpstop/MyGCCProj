#include <x264.h>
#include "options.h"
#include "x264.h"

int x264_init(X264_STR *str) {
	x264_param_t par;
	x264_param_default(&par);
	x264_param_apply_preset(&par, VIDEO_PRESET);
	//x264_param_apply_tune(&par, VIDEO_TUNE);
	par.i_threads = VIDEO_THREADS;
	par.i_width = VIDEO_WIDTH;
	par.i_height = VIDEO_HEIGHT;
	par.i_csp = X264_CSP_I420;
	//TODO
	x264_param_apply_profile(&par, VIDEO_PROFILE);

	str->enc = x264_encoder_open(par);
}

int x264_frame(X264_STR *str, uint8_t **in, int *width, uint32_t pos, unsigned int *out_pos) {
	x264_nal_t *nal;
	int nnal, i, ret;
	x264_picture_t pic_in, pic_out = {0};
	RTMPPacket *p;
	uint8_t *ptr;

	if (in) {
		x264_picture_init(&pic_in);
		pic_in.img.i_csp   = X264_CSP_I420;
		pic_in.img.i_plane = 3;
		for (i = 0; i < 3; i++) {
			pic_in.img.plane[i]    = in[i];
			pic_in.img.i_stride[i] = width[i];
		}
		pic_in.i_pts  = pos;
		pic_in.i_type = X264_TYPE_AUTO;
	}

	do {
		if (x264_encoder_encode(str->enc, &nal, &nnal, in ? &pic_in : NULL, &pic_out) < 0)
			return -1;

		int size = str->sei_size;

		if (!nnal)
			continue;

		for (i = 0; i < nnal; i++)
			size += nals[i].i_payload;

		RTMP_GET_BUFFER(p, size + 5, pos, MEDIUM, VIDEO);

		ptr = p->m_body;

		WRITE_1(ptr, 7 & (pic_out.b_keyframe ? 0x10 : 0x20));
		ptr++;
		INV_WRITE_4(ptr, (pos << 8) | 1);
		ptr+=4;

		if (str->sei_size > 0) {
			memcpy(ptr, str->sei, str->sei_size);
			ptr += str->sei_size;
			str->sei_size = 0;
			free(&str->sei);
		}

		for (i = 0; i < nnal; i++){
			memcpy(ptr, nals[i].p_payload, nals[i].i_payload);
			ptr += nals[i].i_payload;
		}

		RTMP_SET_SIZE(p, ptr - p->m_body);
		rtmp_buf_video[*out_pos] = pkt;
                *out_pos++;
                if(out_pos >= rtmp_buf_video_len) out_pos = 0;
	} while (!in && x264_encoder_delayed_frames(str->enc));
}

int x264_header(X264_STR *str, RTMP *r) {
	x264_nal_t *nal;
	uint8_t *p;
	int nnal, s, i, ret;

	s = x264_encoder_headers(str->enc, &nal, &nnal);
	RTMP_GET_INT_BUFFER(r, s + 5, 0, LARGE, VIDEO);
	p = r->m_write.m_body;
	WRITE_4(p, 0x00000017);
	p += 4;
	WRITE_1(p, 0);
	p++;

	for (i = 0; i < nnal; i++) {
		if (nal[i].i_type == NAL_SEI) {
			str->sei_size = nal[i].i_payload;
			str->sei      = malloc(str->sei_size);
			memcpy(str->sei, nal[i].p_payload, nal[i].i_payload);
			continue;
		}
		memcpy(p, nal[i].p_payload, nal[i].i_payload);
		p += nal[i].i_payload;
	}
	RTMP_SET_INT_SIZE(r, p - r->m_write.m_body);
	RTMP_INT_SEND(r, ret);
	return 0;
}