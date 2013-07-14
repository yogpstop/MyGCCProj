#include <stdio.h>//File
#include <stdlib.h>//str num conv
#include <malloc.h>//Memory
#include <time.h>//Timer
#include <alsa/asoundlib.h>//Midi out
static unsigned char ext[] = {0xF0,0x43,0x73,0x01,0x03,0xF7};
static snd_rawmidi_t* dev;

inline int usage(){
	puts("usage: exe <E-SEQ (has a track) FILE> <Output device> [speed(%)]");
	return -1;
}

int main(int argc, char *argv[]){
	if(argc<3)
		return usage();
	
	FILE *fp;
	unsigned int ptr, i, j;
	unsigned char *data;
	double res = 0;
	struct timespec nBefore, nAfter;

	i = snd_rawmidi_open(NULL, &dev, argv[2], SND_RAWMIDI_SYNC);
	if(i<0)
		return usage();
	
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
	
	if(argc>=4)
		res = 100 / atof(argv[3]);
	if(res<=0)
		res = 1;
	
	snd_rawmidi_write(dev,ext,6);
	
	ptr = *((unsigned int *)&data[0x1B])+0x27;
	
	clock_gettime(CLOCK_MONOTONIC_RAW,&nBefore);
	
	while (ptr<j) {
		switch (data[ptr++]) {
		case 0xF3:
			i = data[ptr++] * res;
			nBefore.tv_sec += i / 1000;
			nBefore.tv_nsec += (i % 1000) * 1000000;
			if(nBefore.tv_nsec >= 1000000000) {
				nBefore.tv_nsec -= 1000000000;
				nBefore.tv_sec += 1;
			}
			clock_gettime(CLOCK_MONOTONIC_RAW,&nAfter);
			if(nAfter.tv_nsec > nBefore.tv_nsec){
				nAfter.tv_nsec -= 1000000000;
				nAfter.tv_sec += 1;
			}
			nAfter.tv_nsec = nBefore.tv_nsec - nAfter.tv_nsec;
			nAfter.tv_sec = nBefore.tv_sec - nAfter.tv_sec;
			nanosleep(&nAfter,NULL);
			break;
		case 0xF4:
			i = ((data[ptr+2]<<7)|data[ptr+1]) * res;
			nBefore.tv_sec += i / 1000;
			nBefore.tv_nsec += (i % 1000) * 1000000;
			if(nBefore.tv_nsec >= 1000000000) {
				nBefore.tv_nsec -= 1000000000;
				nBefore.tv_sec += 1;
			}
			clock_gettime(CLOCK_MONOTONIC_RAW,&nAfter);
			if(nAfter.tv_nsec > nBefore.tv_nsec){
				nAfter.tv_nsec -= 1000000000;
				nAfter.tv_sec += 1;
			}
			nAfter.tv_nsec = nBefore.tv_nsec - nAfter.tv_nsec;
			nAfter.tv_sec = nBefore.tv_sec - nAfter.tv_sec;
			nanosleep(&nAfter,NULL);
			ptr+=2;
			break;
		case 0xFE:
			switch(data[ptr++]){
			case 0x78:
				data[ptr-1]=0xF8;
				snd_rawmidi_write(dev,data+ptr-1,1);
				break;
			case 0x7A:
				data[ptr-1]=0xFA;
				snd_rawmidi_write(dev,data+ptr-1,1);
				break;
			case 0x7C:
				data[ptr-1]=0xFC;
				snd_rawmidi_write(dev,data+ptr-1,1);
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
			snd_rawmidi_write(dev,data+ptr-i,i);
		}
		if(data[ptr-1]==0xF2)
			break;
	}
	ext[4]=0x02;
	snd_rawmidi_write(dev,ext,6);
	
	snd_rawmidi_close(dev);
	dev=NULL;
	
	return 0;
}