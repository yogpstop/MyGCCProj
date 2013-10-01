#include "data_exchange.h"

#ifdef __cplusplus
extern "C" {
#endif

int windows_core_audio_init(void);
THREAD_RET_TYPE windows_core_audio_loop(void*);
int windows_core_audio_sync(void);

#ifdef __cplusplus
}
#endif
