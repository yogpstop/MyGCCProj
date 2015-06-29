#define GLC_MAX_WIDTH 1920
#define GLC_MAX_HEIGHT 1080
#define GLC_COLORDEPTH 4
#define GLC_MAX_PSIZE (GLC_MAX_WIDTH * GLC_MAX_HEIGHT * GLC_COLORDEPTH)
#define GLC_HDR_SIZE (sizeof(LARGE_INTEGER) + sizeof(uint16_t) * 2)
#define GLC_MAX_SIZE (GLC_HDR_SIZE + GLC_MAX_PSIZE)
#define GLC_FPS 30
