#include <stdio.h>
#include <stdlib.h>
#include "flv_core.h"
#define NV(cond, errcode) if (cond) {fputs("It is not valid FLV version 1 file.", stderr);return errcode;}
#define NVB(cond) if (cond) {fputs("It is not valid FLV version 1 file.", stderr);break;}
#pragma pack(1)

int flv_per_tag(char *filename, int (*func)(uint8_t, uint32_t, unsigned int, void*)) {
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
	FILE *fp = fopen(filename, "rb");
	fread(((void*)&tmp), 13, 1, fp);
	NV(tmp.sigver != 0x01564C46, 1)         //Signature Version
	NV(tmp.TypeFlags &  0xFA      , 2)         //TypeFlags
	NV(tmp.dopts0 != 0x0000000009000000, 3) //DataOffset PreviousTagSize0
	while (1) {
		fread(((void*)&tmp), 11, 1, fp);
		if (feof(fp)) break;
		DataSize  = (tmp.b[1] << 16) | (tmp.b[2] << 8) | tmp.b[3];
		tag = malloc(DataSize);
		fread(tag, DataSize, 1, fp);
		if (!func(tmp.b[0], ((tmp.Timestamp & 0xFF) << 16) | ((tmp.Timestamp >> 16) & 0xFF) | (tmp.Timestamp & 0xFF00FF00), DataSize, tag)) free(tag);
		fread(((void*)&tmp), 4, 1, fp);
		buf = (tmp.b[0] << 24) | (tmp.b[1] << 16) | (tmp.b[2] << 8) | tmp.b[3];
		NVB(buf != DataSize + 11)//PreviousTagSize
	}
	fclose(fp);
	return 0;
}
void flv_write_header(FILE *fp, uint8_t typ) {
	struct {
		uint32_t sigver;
		uint8_t TypeFlags;
		uint64_t dopts0;
	} tmp;
	tmp.sigver     = 0x01564C46;         //Signature Version
	tmp.TypeFlags  = typ;                //TypeFlags
	tmp.dopts0     = 0x0000000009000000; //DataOffset PreviousTagSize0
	fwrite(((void*)&tmp), 13, 1, fp);
}
void flv_write_tag(FILE *fp, uint8_t t1, uint32_t Timestamp, unsigned int len, void *dat) {
	union {
		struct {
			uint64_t e1;
			uint32_t StreamID;
		};
		uint32_t e4;
	} tmp;
	tmp.e1 = t1 | ((len & 0xFF0000) >> 8) | ((len & 0xFF00) << 8) | ((len & 0xFF) << 24) |
				((uint64_t)(Timestamp & 0xFF0000) << 16) | ((uint64_t)(Timestamp & 0xFF00FF00) << 32) | ((uint64_t)(Timestamp & 0xFF) << 48);
	tmp.StreamID = 0;
	fwrite(((void*)&tmp), 11, 1, fp);
	fwrite(dat, len, 1, fp);
	len += 11; tmp.e4 = ((len & 0xFF) << 24) | ((len & 0xFF00) << 8) | ((len >> 8) & 0xFF00) | ((len >> 24) & 0xFF);
	fwrite(((void*)&tmp), 4, 1, fp);//PreviousTagSize
}
