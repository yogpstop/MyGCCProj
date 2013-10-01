#include <x264.h>
int main() {
	x264_t *enc;

	x264_param_t par;
	enc = x264_encoder_open(par);

	

	x264_encoder_close(enc);
}

x264_nal_t
x264_picture_t

x264_param_default()
x264_param_default_preset()
x264_param_parse()
x264_param_apply_profile()

x264_encoder_headers()

x264_encoder_encode()
x264_encoder_reconfig()
x264_encoder_delayed_frames()
x264_picture_init()
x264_picture_alloc()
x264_picture_clean()
