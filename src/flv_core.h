#include <stdint.h>
int flv_per_tag(char*, int (*func)(uint8_t, uint32_t, unsigned int, void*));
void flv_write_header(FILE*, uint8_t);
void flv_write_tag(FILE*, uint8_t, uint32_t, unsigned int, void*);
