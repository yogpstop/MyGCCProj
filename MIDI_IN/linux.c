#include <alsa/asoundlib.h>//MIDI_Input
#include <stdio.h>//StandardOutput

inline int usage(){
	puts("usage: exe <Input device>");
	return -1;
}

int main(int argc, char *argv[]) {
	if(argc != 2)
		return -1;
	snd_rawmidi_t* seqfd;
	unsigned char inpacket;
	
	if (snd_rawmidi_open(&seqfd, NULL, argv[1], SND_RAWMIDI_SYNC) < 0)
		return usage();

	while (1) {
		snd_rawmidi_read(seqfd, &inpacket, 1);
		if(inpacket==0xF8||inpacket==0xFE)
			continue;
		if(inpacket>0x7F&&inpacket!=0xF7)
			fputs("\b\n",stdout);
		printf("%02X ",inpacket);
	}
	snd_rawmidi_close(seqfd);
	return 0;
}