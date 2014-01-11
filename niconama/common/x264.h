#include <stdint.h>
#include "rtmp.h"

#define x264_close(str) x264_encoder_close(str.enc)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint8_t *sei;
	int sei_size;
	x264_t *enc;
} X264_STR;

int x264_init(X264_STR *str);
int x264_frame(X264_STR *str, uint8_t **in, int *width, uint32_t pos, unsigned int *out_pos);
int x264_header(X264_STR *str, RTMP *r);

#ifdef __cplusplus
}
#endif
