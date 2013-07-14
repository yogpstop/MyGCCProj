#include <stdio.h>
#include <malloc.h>
#include <windows.h>
static BYTE ext[] = {0xF0,0x43,0x73,0x01,0x03,0xF7};

int usage(){
	puts("usage: exe <E-SEQ (has a track) FILE> [speed(%)]");
	return -1;
}

int main(int argc, char *argv[]){
	if(argc<2)
		return usage();
	
	FILE *fp;
	unsigned int ptr, i, j;
	unsigned char *data;
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
	
	hdr.lpData = (LPSTR)ext;
	hdr.dwBufferLength = 6;
	hdr.dwBytesRecorded = 6;
	midiOutPrepareHeader(hmo, &hdr, sizeof(MIDIHDR));
	midiOutLongMsg(hmo, &hdr, sizeof(MIDIHDR));
	while (!(hdr.dwFlags & MHDR_DONE));
	midiOutUnprepareHeader(hmo, &hdr, sizeof(MIDIHDR));
	
	ptr = *((unsigned int *)&data[0x1B])+0x27;
	
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBefore);
	
	nFreq.QuadPart /= 1000;
	
	if(argc>=3)
		nFreq.QuadPart = nFreq.QuadPart * 100 / atoi(argv[2]);
	
	while (ptr<j) {
		switch (data[ptr++]) {
		case 0xF3:
			nBefore.QuadPart += nFreq.QuadPart*data[ptr++];
			QueryPerformanceCounter(&nAfter);
			i = (nBefore.QuadPart-nAfter.QuadPart)/nFreq.QuadPart;
			if(!(i&0x80000000))
				Sleep(i);
			break;
		case 0xF4:
			nBefore.QuadPart += nFreq.QuadPart*((data[ptr+2]<<7)|data[ptr+1]);
			QueryPerformanceCounter(&nAfter);
			i = (nBefore.QuadPart-nAfter.QuadPart)/nFreq.QuadPart;
			if(!(i&0x80000000))
				Sleep(i);
			ptr+=2;
			break;
		case 0xFE:
			switch(data[ptr++]){
			case 0x78:
				midiOutShortMsg(hmo,0xF8);
				break;
			case 0x7A:
				midiOutShortMsg(hmo,0xFA);
				break;
			case 0x7C:
				midiOutShortMsg(hmo,0xFC);
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
			hdr.lpData = (LPSTR)data+ptr-i;
			hdr.dwBufferLength = i;
			hdr.dwBytesRecorded = i;
			midiOutPrepareHeader(hmo, &hdr, sizeof(MIDIHDR));
			midiOutLongMsg(hmo, &hdr, sizeof(MIDIHDR));
			while (!(hdr.dwFlags & MHDR_DONE));
			midiOutUnprepareHeader(hmo, &hdr, sizeof(MIDIHDR));
		}
		if(data[ptr-1]==0xF2)
			break;
	}
	ext[4]=0x02;
	hdr.lpData = (LPSTR)ext;
	hdr.dwBufferLength = 6;
	hdr.dwBytesRecorded = 6;
	midiOutPrepareHeader(hmo, &hdr, sizeof(MIDIHDR));
	midiOutLongMsg(hmo, &hdr, sizeof(MIDIHDR));
	while (!(hdr.dwFlags & MHDR_DONE));
	midiOutUnprepareHeader(hmo, &hdr, sizeof(MIDIHDR));
	
	midiOutReset(hmo);
	midiOutClose(hmo);
	
	return 0;
}