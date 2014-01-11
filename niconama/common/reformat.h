#include <libswscale/swscale.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RESIZE_init(str) sws_getCachedContext(&str.c, \
	str.src_width, str.src_height, AV_PIX_FMT_RGB24, \
	str.dst_width, str.dst_height, AV_PIX_FMT_YUV420P, \
	str.algorithm, NULL, NULL, NULL)

#define RESIZE_do(str,src,srcl,dst,dstl) sws_scale(&str.c,src,srcl,0,str->src_height,dst,dstl)

#define RESIZE_exit(str) sws_freeContext(str.c);

typedef struct {
	int src_width;
	int src_height;
	int dst_width;
	int dst_height;
	int algorithm;

	struct SwsContext c;
} RESIZE_STR;


#ifdef __cplusplus
}
#endif
