#include <stdio.h>
#include <malloc.h>
static unsigned char ext[] = {0xF0,0x43,0x73,0x01,0x03,0xF7};

#if _WIN32

#include <windows.h>

#define WRITE(ptr, len) \
	hdr.lpData = (LPSTR)ptr; \
	hdr.dwBufferLength = len; \
	hdr.dwBytesRecorded = len; \
	midiOutPrepareHeader(hmo, &hdr, sizeof(MIDIHDR)); \
	midiOutLongMsg(hmo, &hdr, sizeof(MIDIHDR)); \
	while (!(hdr.dwFlags & MHDR_DONE)); \
	midiOutUnprepareHeader(hmo, &hdr, sizeof(MIDIHDR));

#define SLEEP(time) \
	nBefore.QuadPart += (time) * res; \
	QueryPerformanceCounter(&nAfter); \
	nAfter.QuadPart = (nBefore.QuadPart - nAfter.QuadPart) / res; \
	if(nAfter.QuadPart >= 0) \
		Sleep(i);

#define WRITE_S(val) midiOutShortMsg(hmo, val);

#else

#include <stdlib.h>
#include <time.h>
#include <alsa/asoundlib.h>

#define WRITE(ptr, len) snd_rawmidi_write(dev, ptr, len);

#define SLEEP(time) \
	i = (time) * res; \
	nBefore.tv_sec += i / 1000; \
	nBefore.tv_nsec += (i % 1000) * 1000000; \
	if(nBefore.tv_nsec >= 1000000000) { \
		nBefore.tv_nsec -= 1000000000; \
		nBefore.tv_sec += 1; \
	} \
	clock_gettime(CLOCK_MONOTONIC_RAW, &nAfter); \
	if(nAfter.tv_nsec > nBefore.tv_nsec){ \
		nAfter.tv_nsec -= 1000000000; \
		nAfter.tv_sec += 1; \
	} \
	nAfter.tv_nsec = nBefore.tv_nsec - nAfter.tv_nsec; \
	nAfter.tv_sec = nBefore.tv_sec - nAfter.tv_sec; \
	if(nAfter.tv_sec >= 0) \
		nanosleep(&nAfter, NULL);

#define WRITE_S(val) \
	data[ptr - 1] = val; \
	snd_rawmidi_write(dev, data + ptr - 1, 1);

#endif

inline int usage(){
#if _WIN32
	puts("usage: exe <E-SEQ (has a track) FILE> [speed(%)]");
#else
	puts("usage: exe <E-SEQ (has a track) FILE> <Output device> [speed(%)]");
#endif
	return -1;
}

int main(int argc, char *argv[]){
	if(argc < (_WIN32 ? 2 : 3))
		return usage();
	
	FILE *fp;
	unsigned int ptr, i, j;
	unsigned char *data;
	double res = 0;
#if _WIN32
	LARGE_INTEGER nFreq, nBefore, nAfter;
	MIDIOUTCAPS moc;
	HMIDIOUT hmo;
	MIDIHDR hdr = {};
	
	j = midiOutGetNumDevs();
	for(i=0; i<j; i++){
		midiOutGetDevCaps(i, &moc, sizeof(moc));
		printf("[%d] %s\n", i, moc.szPname);
	}
	printf("\n");
	while (1) {
		printf("Enter output device number: ");
		j = scanf("%d", &i);
		if (j==0) {
			scanf("%*s");
			continue;
		}
		if (midiOutOpen(&hmo, i, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
			printf("Opening MIDI OUT Device is failed.\n");
			continue;
		}
		break;
	}
#else
	struct timespec nBefore, nAfter;
	snd_rawmidi_t* dev;

	i = snd_rawmidi_open(NULL, &dev, argv[2], SND_RAWMIDI_SYNC);
	if(i<0)
		return usage();
#endif
	
	if((fp = fopen(argv[1],"rb")) == NULL)
		return usage();
	fseek(fp,0,SEEK_END);
	j = ftell(fp);
	fseek(fp,0,SEEK_SET);
	data = malloc(j);
	if(data == NULL){
		puts("can't alloc memory.");
		return -1;
	}
	j = fread(data,1,j,fp);
	fclose(fp);
	if(data[0x23]!=0x01)
		return usage();
	
	if(argc >= (_WIN32 ? 3 : 4))
#if _WIN32
		res = atof(argv[2]) / 10;
#else
		res = 100 / atof(argv[3]);
#endif
	if(res<=0)
		res = 1;
	
	WRITE(ext, 6)
	
	ptr = *((unsigned int *)&data[0x1B])+0x27;
	
#if _WIN32
	QueryPerformanceFrequency(&nFreq);
	res *= nFreq.QuardPart;
	QueryPerformanceCounter(&nBefore);
#else
	clock_gettime(CLOCK_MONOTONIC_RAW,&nBefore);
#endif
	
	while (ptr<j) {
		switch (data[ptr++]) {
		case 0xF3:
			SLEEP(data[ptr++])
			break;
		case 0xF4:
			SLEEP((data[ptr+2]<<7)|data[ptr+1])
			ptr+=2;
			break;
		case 0xFE:
			switch(data[ptr++]){
			case 0x78:
				WRITE_S(0xF8)
				break;
			case 0x7A:
				WRITE_S(0xFA)
				break;
			case 0x7C:
				WRITE_S(0xFC)
				break;
			}
			break;
		case 0xF9:
		case 0xFB:
			ptr++;
		case 0xF1:
		case 0xFF:
			ptr++;
		case 0xF2:
		case 0xF6:
			break;
		default:
			ptr--;
			i=1;
			while(data[++ptr]<0x80)
				i++;
			if(data[ptr]==0xF7){
				ptr++;
				i++;
			}
			WRITE(data+ptr-i, i)
		}
		if(data[ptr-1]==0xF2)
			break;
	}
	ext[4]=0x02;
	WRITE(ext, 6)
	
#if _WIN32
	midiOutReset(hmo);
	midiOutClose(hmo);
#else
	snd_rawmidi_close(dev);
	dev=NULL;
#endif
	
	return 0;
}