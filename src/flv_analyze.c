#include <stdio.h>
#include <stdint.h>
#define TAG_TYPE_AUDIO 8
#define TAG_TYPE_VIDEO 9
#define TAG_TYPE_SCRIPT 18
#define read8(buf, fp) \
	fread(((uint8_t*)&buf)+7,1,1,fp); fread(((uint8_t*)&buf)+6,1,1,fp); \
	fread(((uint8_t*)&buf)+5,1,1,fp); fread(((uint8_t*)&buf)+4,1,1,fp); \
	fread(((uint8_t*)&buf)+3,1,1,fp); fread(((uint8_t*)&buf)+2,1,1,fp); \
	fread(((uint8_t*)&buf)+1,1,1,fp); fread(((uint8_t*)&buf),1,1,fp);
#define read4(buf, fp) \
	fread(((uint8_t*)&buf)+3,1,1,fp); fread(((uint8_t*)&buf)+2,1,1,fp); \
	fread(((uint8_t*)&buf)+1,1,1,fp); fread(((uint8_t*)&buf),1,1,fp);
#define read3(buf, fp) buf = 0; fread(((uint8_t*)&buf)+2,1,1,fp); \
	fread(((uint8_t*)&buf)+1,1,1,fp); fread(((uint8_t*)&buf),1,1,fp);
#define read2(buf, fp) buf = 0; \
	fread(((uint8_t*)&buf)+1,1,1,fp); fread(((uint8_t*)&buf),1,1,fp);
#define read1(buf, fp) buf = 0; fread(((uint8_t*)&buf),1,1,fp);

void script(FILE *fp, FILE *fp_scr, unsigned int depth, unsigned int rest,
			unsigned int has_title) {
	uint32_t buf;
	double dbuf;
	int i;
	if (has_title) {
		read2(buf, fp);
		for (i = 0; i < depth; i++)
			fputc('\t',fp_scr);
		for (i = 0; i < buf; i++) {
			fread(&dbuf, 1, 1, fp);
			fwrite(&dbuf, 1, 1, fp_scr);
		}
		fputc('\n', fp_scr);
	}
	depth++;
	read1(buf, fp);
	switch (buf) {
	case 0:
		read8(dbuf, fp);
		for (i = 0; i < depth; i++)
			fputc('\t', fp_scr);
		fprintf(fp_scr, "%f", dbuf);
		break;
	case 1:
		read1(buf, fp);
		for (i = 0; i < depth; i++)
			fputc('\t', fp_scr);
		fprintf(fp_scr, "%d", buf);
		break;
	case 2:
		read2(buf, fp);
		for (i = 0; i < depth; i++)
			fputc('\t',fp_scr);
		for (i = 0; i < buf; i++) {
			fread(&dbuf, 1, 1, fp);
			fwrite(&dbuf, 1, 1, fp_scr);
		}
		break;
	case 8:
		read4(buf, fp);
	case 3:
		script(fp, fp_scr, depth, 0, 1);
		break;
	case 7:
		read2(buf, fp);
		for(i = 0; i < depth; i++)
			fputc('\t', fp_scr);
		fprintf(fp_scr, "%d", buf);
		break;
	case 11:
		read8(dbuf, fp);
		read2(buf, fp);
		for(i = 0; i < depth; i++)
			fputc('\t', fp_scr);
		fprintf(fp_scr, "%f+%d", dbuf, buf);
		break;
	case 12:
		read4(buf, fp);
		for(i = 0; i < depth; i++)
			fputc('\t', fp_scr);
		for(i = 0; i < buf; i++) {
			fread(&dbuf, 1, 1, fp);
			fwrite(&dbuf, 1, 1, fp_scr);
		}
		break;
	case 9:
		fputc('\n', fp_scr);
		return;
	case 10:
		read4(buf, fp);
		script(fp, fp_scr, depth - 1, buf, 0);
		break;
	}
	fputc('\n', fp_scr);
	if (!has_title) {
		rest--;
		if (!rest) return;
	}
	depth--;
	script(fp, fp_scr, depth, rest, has_title);
	return;
}

int main(int argc, char **argv) {
	uint32_t buf, DataSize, Timestamp, StreamID, i;
	uint8_t FMS, Filter, TagType, Format;
	FILE *fp, *fpd, *fp_aud, *fp_vid, *fp_scr, *fp_etc;
	fp = fopen(argv[1], "rb");
	read4(buf, fp);//Signature Version
	if (buf != 0x464C5601) {
		fputs("It is not valid FLV version 1 file.", stderr);
		return -1;
	}
	read1(buf, fp);//TypeFlags
	if (buf & 0xFA) {
		fputs("It is not valid FLV version 1 file.", stderr);
		return -1;
	}
	fpd = fopen("file_desc.txt", "wb");
	if (buf & 0x4)
		fputs("This FLV file has video section.\n", fpd);
	if (buf & 0x1)
		fputs("This FLV file has audio section.\n", fpd);
	fclose(fpd);
	read4(buf, fp);//DataOffset
	if (buf != 9) {
		fputs("It is not valid FLV version 1 file.", stderr);
		return -1;
	}
	read4(buf, fp);//PreviousTagSize0
	if (buf != 0) {
		fputs("It is not valid FLV version 1 file.", stderr);
		return -1;
	}
	fp_aud = fopen("file_audio.csv", "wb");
	fp_vid = fopen("file_video.csv", "wb");
	fp_scr = fopen("file_script.csv", "wb");
	fp_etc = fopen("file_others.csv", "wb");
	fputs("FMS, f, TagType, DataSize, Timestamp, StreamID, "
		"SoundFormat, Rate, Size, Type, AACPktType\n", fp_aud);
	fputs("FMS, f, TagType, DataSize, Timestamp, StreamID, "
		"FrameType, CodecID, AVCPktType, CmpsTime, seq\n", fp_vid);
	fputs("FMS, f, TagType, DataSize, Timestamp, StreamID\n", fp_scr);
	fputs("FMS, f, TagType, DataSize, Timestamp, StreamID\n", fp_etc);
	while (1) {
		read1(buf, fp);
		if (feof(fp)) break;
		FMS = buf >> 6;
		Filter = (buf >> 5) & 1;
		TagType = buf & 0x1F;
		read3(DataSize, fp);//DataSize
		read3(Timestamp, fp);//Timestamp
		fread(((uint8_t*)&Timestamp) + 3, 1, 1, fp);//TimestampExtended
		read3(StreamID, fp);//StreamID

		if (TagType == TAG_TYPE_AUDIO) {
			fprintf(fp_aud, "%3d, %1d,   audio, %8d, %9d, %8d, ", FMS, Filter,
						DataSize, Timestamp, StreamID);
			read1(buf, fp);
			Format = buf >> 4;
			switch (Format) {
			case 0:
				fputs(" LPCM pf ed, ", fp_aud);
				break;
			case 1:
				fputs("ADPCM      , ", fp_aud);
				break;
			case 2:
				fputs("        MP3, ", fp_aud);
				break;
			case 3:
				fputs(" LPCM    LE, ", fp_aud);
				break;
			case 4:
				fputs("Nlms 16k mo, ", fp_aud);
				break;
			case 5:
				fputs("Nlms  8k mo, ", fp_aud);
				break;
			case 6:
				fputs(" Nellymoser, ", fp_aud);
				break;
			case 7:
				fputs("G.711 A-law, ", fp_aud);
				break;
			case 8:
				fputs("G.711 mu-l , ", fp_aud);
				break;
			case 9:
				fputs("   reserved, ", fp_aud);
				break;
			case 10:
				fputs("        AAC, ", fp_aud);
				break;
			case 11:
				fputs("      Speex, ", fp_aud);
				break;
			case 14:
				fputs(" 8kHz   MP3, ", fp_aud);
				break;
			case 15:
				fputs("   dev spec, ", fp_aud);
				break;
			default:
				fprintf(fp_aud, "%11d, ", Format);
			}
			switch ((buf >> 2) & 3) {
			case 0:
				fputs("5.5k, ", fp_aud);
				break;
			case 1:
				fputs(" 11k, ", fp_aud);
				break;
			case 2:
				fputs(" 22k, ", fp_aud);
				break;
			case 3:
				fputs(" 44k, ", fp_aud);
				break;
			}
			if (buf & 0x2)
				fputs("16bt, ", fp_aud);
			else
				fputs(" 8bt, ", fp_aud);
			if (buf & 0x1)
				fputs("mono, ", fp_aud);
			else
				fputs(" ste, ", fp_aud);
			if (Format == 10) {
				read1(buf, fp);//AACPacketType
				if (buf == 1)
					fputs("   seq hdr\n", fp_aud);
				else if (buf == 0)
					fputs("   AAC raw\n", fp_aud);
				else
					fprintf(fp_aud, "%10d\n", buf);
				fseek(fp, DataSize - 2, SEEK_CUR);
			} else {
				fputs("          \n", fp_aud);
				fseek(fp, DataSize - 1, SEEK_CUR);
			}
		} else if (TagType == TAG_TYPE_VIDEO) {
			fprintf(fp_vid, "%3d, %1d,   video, %8d, %9d, %8d, ", FMS, Filter,
						DataSize, Timestamp, StreamID);
			read1(buf, fp);
			TagType = buf >> 4;
			switch (TagType) {
			case 1:
				fputs("key frame, ", fp_vid);
				break;
			case 2:
				fputs("int frame, ", fp_vid);
				break;
			case 3:
				fputs("nopos int, ", fp_vid);
				break;
			case 4:
				fputs("gened key, ", fp_vid);
				break;
			case 5:
				fputs("inf/cmd  , ", fp_vid);
				break;
			default:
				fprintf(fp_vid, "%10d, ", TagType);
				break;
			}
			Format = buf & 0xF;
			switch (Format) {
			case 2:
				fputs("  H.263, ", fp_vid);
				break;
			case 3:
				fputs("scr vid, ", fp_vid);
				break;
			case 4:
				fputs("VP6    , ", fp_vid);
				break;
			case 5:
				fputs("VP6 alp, ", fp_vid);
				break;
			case 6:
				fputs("scr v2 , ", fp_vid);
				break;
			case 7:
				fputs("    AVC, ", fp_vid);
				break;
			default:
				fprintf(fp_vid, "%7d, ", Format);
				break;
			}
			if (Format == 7) {
				read1(buf, fp);//AVCPacketType
				switch (buf) {
				case 0:
					fputs("   seq hdr, ", fp_vid);
					break;
				case 1:
					fputs("      NALU, ", fp_vid);
					break;
				case 2:
					fputs("end of seq, ", fp_vid);
					break;
				default:
					fprintf(fp_vid, "%10d, ", buf);
					break;
				}
				read3(buf, fp);//CompositionTime
				fprintf(fp_vid, "%8d, ", buf);
			} else {
				fputs("          ,         , ", fp_vid);
			}
			if (!Filter && TagType == 5) {
				read1(buf, fp);//VideoTagBody
				switch (buf) {
				case 0:
					fputs(" st\n", fp_vid);
					break;
				case 1:
					fputs("end\n", fp_vid);
					break;
				default:
					fprintf(fp_vid, "%3d\n", buf);
					break;
				}
				fseek(fp, -1, SEEK_CUR);
			} else {
				fputs("   \n", fp_vid);
			}
			if (Format == 7)
				fseek(fp, -4, SEEK_CUR);
			fseek(fp, DataSize - 1, SEEK_CUR);
		} else if (TagType == TAG_TYPE_SCRIPT) {
			fprintf(fp_scr, "%3d, %1d,  script, %8d, %9d, %8d\n", FMS, Filter,
						DataSize, Timestamp, StreamID);
			if (Filter)
				fseek(fp, DataSize, SEEK_CUR);
			else {
				read1(buf, fp);//Type
				if (buf != 2) {
					fputs("It is not valid FLV version 1 file.", stderr);
					return -1;
				}
				read2(buf, fp);//StringLength
				fputc('\t', fp_scr);
				for (i = 0; i < buf; i++) {
					fread(&Timestamp, 1, 1, fp);
					fwrite(&Timestamp, 1, 1, fp_scr);
				}
				fputc('\n', fp_scr);
				read1(buf, fp);//Type
				if (buf != 8) {
					fputs("It is not valid FLV version 1 file.", stderr);
					return -1;
				}
				read4(buf, fp);//ECMAArrayLength
				script(fp, fp_scr, 2, 0, 1);
			}
		} else {
			fprintf(fp_etc, "%3d, %1d, %7d, %8d, %9d, %8d\n", FMS, Filter,
						TagType, DataSize, Timestamp, StreamID);
			fseek(fp, DataSize, SEEK_CUR);
		}

		read4(buf, fp);//PreviousTagSize
		if (buf != DataSize + 11) {
			fputs("It is not valid FLV version 1 file.", stderr);
			return -1;
		}
	}
	fclose(fp);
	fclose(fp_aud);
	fclose(fp_vid);
	fclose(fp_scr);
	fclose(fp_etc);
	return 0;
}
