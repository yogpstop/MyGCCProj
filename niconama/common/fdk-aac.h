#pragma once

#include "data_exchange.h"

#ifdef __cplusplus
extern "C" {
#endif

int fdk_aac_init(void);
int fdk_aac_header(RTMP*);
THREAD_RET_TYPE fdk_aac_loop(void*);

#ifdef __cplusplus
}
#endif
