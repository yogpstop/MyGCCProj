#include <libswresample/swresample.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RESAMPLE_init1(str) str.c = swr_alloc()

#define RESAMPLE_init2(str) swr_init(&str.c)

#define RESAMPLE_do(str,src,srcl,dst,dstl) sws_scale(&str.c,src,srcl,0,str->src_height,dst,dstl)

#define RESAMPLE_exit(str) sws_freeContext(str.c);

typedef struct {
	int src_width;
	int src_height;
	int dst_width;
	int dst_height;
	int algorithm;

	struct SwrContext *c;
} RESIZE_STR;


#ifdef __cplusplus
}
#endif
