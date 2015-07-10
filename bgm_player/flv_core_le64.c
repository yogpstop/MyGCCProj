#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#define NV(cond, errcode) if (cond) {fputs("It is not valid FLV version 1 file.", stderr);return errcode;}
#define NVB(cond) if (cond) {fputs("It is not valid FLV version 1 file.", stderr);break;}
#pragma pack(1)

int flv_per_tag(FILE *fp, void *ctx, int (*func)(uint8_t, uint32_t, unsigned int, void*, void*)) {
	uint32_t buf, DataSize; void *tag;
	union {
		struct {
			uint32_t sigver;
			uint8_t TypeFlags;
			uint64_t dopts0;
		};
		struct {
			uint8_t b[4];
			uint32_t Timestamp;
			uint32_t StreamID;
		};
	} tmp;
	buf = fread(((void*)&tmp), 13, 1, fp);
	NV(tmp.sigver != 0x01564C46, 1)         //Signature Version
	NV(tmp.TypeFlags &  0xFA      , 2)         //TypeFlags
	NV(tmp.dopts0 != 0x0000000009000000, 3) //DataOffset PreviousTagSize0
	while (1) {
		buf = fread(((void*)&tmp), 11, 1, fp);
		if (feof(fp)) break;
		DataSize  = (tmp.b[1] << 16) | (tmp.b[2] << 8) | tmp.b[3];
		tag = malloc(DataSize);
		buf = fread(tag, DataSize, 1, fp);
		if (!func(tmp.b[0], ((tmp.Timestamp & 0xFF) << 16) | ((tmp.Timestamp >> 16) & 0xFF) | (tmp.Timestamp & 0xFF00FF00), DataSize, tag, ctx)) free(tag);
		buf = fread(((void*)&tmp), 4, 1, fp);
		buf = (tmp.b[0] << 24) | (tmp.b[1] << 16) | (tmp.b[2] << 8) | tmp.b[3];
		NVB(buf != DataSize + 11)//PreviousTagSize
	}
	fclose(fp);
	return 0;
}
