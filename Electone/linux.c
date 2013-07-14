#include <curses.h>
#include <alsa/asoundlib.h>

#define UV1  0x0001
#define UV2  0x0002
#define LV1  0x0004
#define LV2  0x0008
#define RV1  0x0010
#define PV1  0x0020
#define PV2  0x0040
#define RTM  0x0080
#define UK   0x0100
#define LK   0x0200
#define PK   0x0400
#define GLB  0x0800
#define TC   0x1000
#define RP   0x2000
#define NONE 0x4000

#define VOICE  (UV1|UV2|LV1|LV2|RV1|PV1|PV2)
#define KEY (UK|LK|PK)
#define ALL (VOICE|RTM|KEY|GLB|TC|RP|NONE)

#define GROUP(X) (X-1)
#define NEUTRAL(X) (X+2)

#define PERCUSSION 7

#define VIBRATO 8

#define AC 9
#define ABC 10
#define MOC 11

#define REVERB 12
#define TREMOLO 13
#define SYMPHONIC 14
#define DELAY 15
#define FLANGER 16
#define DISTORTION 17

#define CURRENTVALUE 19
#define BAR 20
#define MSG(X) (X+20)

struct item {
	const int top;
	const int group;
	const char *name;
	const int min;
	const int max;
	const char *minS;
	const char *maxS;
	const int key;
	const char Laddr;
};

const struct item items[] = {
	{ALL,GROUP(1),"UV1",-1,1,NULL,NULL,KEY_F(1)},
	{ALL,GROUP(1),"UV2",-1,2,NULL,NULL,KEY_F(2)},
	{ALL,GROUP(1),"LV1",-1,3,NULL,NULL,KEY_F(3)},
	{ALL,GROUP(1),"LV2",-1,4,NULL,NULL,KEY_F(4)},
	{ALL,GROUP(1),"RV1",-1,5,NULL,NULL,KEY_F(5)},
	{ALL,GROUP(1),"PV1",-1,6,NULL,NULL,KEY_F(6)},
	{ALL,GROUP(1),"PV2",-1,7,NULL,NULL,KEY_F(7)},
	{ALL,GROUP(2),"RTM",-1,8,NULL,NULL,KEY_F(8)},
	{ALL,GROUP(2),"UK ",-1,9,NULL,NULL,KEY_F(9)},
	{ALL,GROUP(2),"LK ",-1,10,NULL,NULL,KEY_F(10)},
	{ALL,GROUP(2),"PK ",-1,11,NULL,NULL,KEY_F(11)},
	{ALL,GROUP(2),"GLB",-1,12,NULL,NULL,KEY_F(12)},
	{VOICE,VIBRATO,"USER",0,1,"PRESET","USER",'T',0x19},
	{VOICE,VIBRATO,"Delay",0,0x7F,"Short","Long",'Y',0x1A},
	{VOICE,VIBRATO,"Depth",0,0x7F,"MIN","MAX",'U',0x1B},
	{VOICE,VIBRATO,"Speed",0,0x7F,"SLOW","FAST",'I',0x1C},
	{VOICE,VIBRATO,"Touch",0,1,"OFF","ON",'O',0x1E},
	{LK|PK,PERCUSSION,"ON/OFF",0,1,"OFF","ON",'P',0x10},
	{LK|PK,PERCUSSION,"Mode",0,8,"Preset","UserAssign1-8",'@',0x11},
	{RTM,AC,"Pattern",0,3,"Type1,Type2,Type3,Type4",NULL,'A',0x10},
	{RTM,AC,"Mute",0,1,"OFF","MUTE",'S',0x11},
	{RTM,AC,"Volume",0,0x7F,"MIN","MAX",'D',0x12},
	{RTM,AC,"Reverb",0,0x7F,"MIN","MAX",'F',0x13},
	{RTM,ABC,"Mode",0,3,"OFF,SingleFinger,Fingerd,CustomABC",NULL,'G',0x00},
	{RTM,ABC,"LKMemory",0,1,"OFF","ON",'H',0x01},
	{RTM,ABC,"PKMemory",0,1,"OFF","ON",'J',0x02},
	{RTM,MOC,"Mode",0,3,"OFF,Close2Note,Close3Note,PassBOpen",NULL,'K',0x00},
	{RTM,MOC,"KneeControl",0,1,"OFF","ON",'L',0x01},
	{VOICE|RTM,NEUTRAL(1),"Volume",0,0x7F,"MIN","MAX",'1',0x11},
	{VOICE|RTM,NEUTRAL(1),"Reverb",0,0x7F,"MIN","MAX",'2',0x12},
	{VOICE,NEUTRAL(1),"ToneColor",-2,12,NULL,NULL,'3'},
	{VOICE,NEUTRAL(1),"Brilliance",0,0x7F,"Brilliant","Mellow",'4',0x13},
	{RV1|PV1|PV2,NEUTRAL(1),"TOLOWER",0,1,"OFF","ON",'5',0x1F},
	{VOICE,NEUTRAL(2),"Feat",0,4,"Preset,16Feet,8Feet,4Feet,2Feet",NULL,'6',0x14},
	{VOICE,NEUTRAL(2),"Pan",0,0x7F,"Right","Left",'7',0x15},
	{VOICE,NEUTRAL(2),"InitialTouchSense",0,0x7F,"Shallow","Deep",'8',0x16},
	{VOICE,NEUTRAL(2),"AfterTouchSense",0,0x7F,"Shallow","Deep",'9',0x17},
	{RV1,NEUTRAL(3),"Slide",0,2,"OFF,ON,KneeControl",NULL,'0',0x20},
	{RV1,NEUTRAL(3),"SlideSpeed",0x02,0x7F,"Slow","Fast",'-',0x21},
	{RV1,NEUTRAL(3),"Detune",0,0x24,"Normal","TuneUP",'^',0x22},
	{UV1|UV2|RV1|PV1|PV2,NEUTRAL(3),"SecondExp",0,1,"OFF","ON",'Q',0x23},
	{UV1|UV2|LV1|LV2|RV1,NEUTRAL(3),"Glide",0,1,"OFF","ON",'W',0x24},
	{VOICE,NEUTRAL(4),"EffectSelect",0,6,"Preset,OFF,Tremolo,Symphonic,Delay,Flanger,Distortion",NULL,'E',0x3F},
	{KEY,NEUTRAL(1),"Sustain",0,1,"OFF","ON",'3',0x00},
	{KEY,NEUTRAL(1),"SustainLength",0,0x0C,"Short","Long",'4',0x01},
	{RTM,NEUTRAL(1),"RhythmPattern",-1,20,NULL,NULL,'3'},
	{RTM,NEUTRAL(1),"AutoVariation",0,1,"OFF","ON",'4',0x14},
	{RTM,NEUTRAL(2),"RhythmBalance",0,0x7F,"Cymbal","Drums",'5',0x15},
	{RTM,NEUTRAL(2),"SecondExTempoControl",0,1,"OFF","ON",'6',0x16},
	{RTM,NEUTRAL(2),"FootSwRhythmMode",0,3,"OFF,RhythmStop,Ending,FillIn",NULL,'7',0x17},
	{GLB,NEUTRAL(1),"Disable",0,1,"OFF","ON",'3',0x00},
	{GLB,NEUTRAL(1),"FluteCouplerAttackMode",0,1,"EachKey","FirstNewKey",'4',0x01},
	{GLB,NEUTRAL(1),"Transpose",0x3A,0x46,"KeyDown","KeyUp",'5',0x02},
	{GLB,NEUTRAL(2),"SecondExpVariWidth",1,0x0C,"100","1200",'6',0x03},
	{GLB,NEUTRAL(2),"FootSwMode",0,0x03,"OFF,Rhythm,Glide,Tremolo",NULL,'7',0x04},
	{GLB,NEUTRAL(2),"Pitch",0x3C,0x4F,"PitchDown","PitchUp",'8',0x05},
	{GLB,NEUTRAL(3),"GlideSpeed",0x01,0x05,"Slow","Fast",'9',0x06},
	{GLB,NEUTRAL(3),"ManualBalance",0x34,0x4C,"Upper","Lower",'0',0x07},
	{GLB,NEUTRAL(3),"LIT",0,1,"Mode1","Mode2",'-',0x09},
	{GLB,NEUTRAL(3),"EXPRESSION I/E",0,1,"Internal","External",'^',0x0A},
	{TC,NEUTRAL(1),"Strings",0,11,"",NULL,'1',0x00},
	{TC,NEUTRAL(1),"Violin",0,7,"",NULL,'2',0x01},
	{TC,NEUTRAL(1),"ContraBass",0,4,"",NULL,'3',0x02},
	{TC,NEUTRAL(1),"Brass",0,7,"",NULL,'4',0x03},
	{TC,NEUTRAL(1),"Horn",0,4,"",NULL,'5',0x04},
	{TC,NEUTRAL(2),"Trumpet",0,11,"",NULL,'6',0x05},
	{TC,NEUTRAL(2),"Tuba",0,8,"",NULL,'7',0x06},
	{TC,NEUTRAL(2),"Flute",0,8,"",NULL,'8',0x07},
	{TC,NEUTRAL(2),"Oboe",0,4,"",NULL,'9',0x08},
	{TC,NEUTRAL(2),"Clarinet",0,4,"",NULL,'0',0x09},
	{TC,NEUTRAL(2),"Saxphone",0,10,"",NULL,'-',0x0A},
	{TC,NEUTRAL(3),"Tutti",0,6,"",NULL,'^',0x0B},
	{TC,NEUTRAL(3),"Chorus",0,5,"",NULL,'Q',0x0C},
	{TC,NEUTRAL(3),"Harmonica",0,1,"",NULL,'W',0x0D},
	{TC,NEUTRAL(3),"Organ",0,13,"",NULL,'E',0x0E},
	{TC,NEUTRAL(3),"OrganBass",0,3,"",NULL,'R',0x0F},
	{TC,NEUTRAL(4),"Piano",0,8,"",NULL,'T',0x10},
	{TC,NEUTRAL(4),"Guitar",0,15,"",NULL,'Y',0x11},
	{TC,NEUTRAL(4),"Vibraphone",0,8,"",NULL,'U',0x12},
	{TC,NEUTRAL(4),"ELectricBase",0,6,"",NULL,'I',0x13},
	{TC,NEUTRAL(4),"Cosmic",0,15,"",NULL,'O',0x14},
	{TC,NEUTRAL(4),"USER",0,15,"",NULL,'P',0x15},
	{RP,NEUTRAL(1),"March",0,8,"",NULL,'1',0x01},
	{RP,NEUTRAL(1),"Waltz",9,17,"",NULL,'2',0x01},
	{RP,NEUTRAL(1),"Swing",18,26,"",NULL,'3',0x01},
	{RP,NEUTRAL(1),"Bounce",27,31,"",NULL,'4',0x01},
	{RP,NEUTRAL(1),"SlowRock",32,34,"",NULL,'5',0x01},
	{RP,NEUTRAL(2),"Tango",35,37,"",NULL,'6',0x01},
	{RP,NEUTRAL(2),"Latin1",38,42,"",NULL,'7',0x01},
	{RP,NEUTRAL(2),"Latin2",43,48,"",NULL,'8',0x01},
	{RP,NEUTRAL(2),"8Beat",49,57,"",NULL,'9',0x01},
	{RP,NEUTRAL(2),"16Beat",58,65,"",NULL,'0',0x01},
	{RP,NEUTRAL(2),"USER",67,98,"",NULL,'-',0x01},
	{VOICE|KEY|GLB,REVERB,"Type",0,2,"Room,Hall,Church",NULL,'Z',0x40},
	{VOICE|KEY|GLB,REVERB,"Length",0,-1,"","",'X',0x41},
	{VOICE|KEY|GLB,REVERB,"Depth",0,-1,"","",'C',0x42},
	{VOICE|KEY|GLB,TREMOLO,"ON/OFF",0,1,"OFF","ON",'V',0x48},
	{VOICE|KEY|GLB,TREMOLO,"Speed",0,-1,"4.75Hz","7.77Hz",'B',0x49},
	{VOICE|KEY|GLB,TREMOLO,"ChorusMode",0,1,"SLOW","STOP",'N',0x4A},
	{VOICE|KEY|GLB,SYMPHONIC,"Mode",0,1,"SYMPHONIC","CELESTE",'M',0x50},
	{VOICE|KEY|GLB,DELAY,"Time",0,-1,"5ms","956ms",'A',0x58},
	{VOICE|KEY|GLB,DELAY,"Feedback",0,-1,"0.2%","46.9%",'S',0x59},
	{VOICE|KEY|GLB,DELAY,"Balance",0,-1,"0%","100%",'D',0x5A},
	{VOICE|KEY|GLB,DELAY,"Mode",0,3,"MONO,ST.1,ST.2,ST.3",NULL,'F',0x5B},
	{VOICE|KEY|GLB,FLANGER,"Speed",0,-1,"0Hz","12.1Hz",'G',0x60},
	{VOICE|KEY|GLB,FLANGER,"Feedback",0,-1,"0.6%","94.0%",'H',0x61},
	{VOICE|KEY|GLB,FLANGER,"Depth",0,-1,"37.8%","87.8%",'J',0x62},
	{VOICE|KEY|GLB,DISTORTION,"Level",0,-1,"0dB","21.8dB",'K',0x68},
	{VOICE|KEY|GLB,DISTORTION,"Hipass",0,-1,"20Hz","1036Hz",'L',0x69}
};


int currentMode=21;
int currentKey=0;
int currentValue=0;
int sub=FALSE;
int ready;
static snd_rawmidi_t* dev;

unsigned char data1[] = {0xF0,0x43,0x70,0x78,0x44,0x00,0x00,0x00,0x00,0xF7};
unsigned char data2[] = {0xF0,0x43,0x70,0x78,0x44,0x00,0x00,0x00,0x00,0x00,0xF7};
#define ADDR(X) (X+4)
#define DATA 8

void draw(){
	ready=0;
	int i;
	int prev=-1;
	int curM=1<<((13<=currentMode&&currentMode<=19)?12:((20<=currentMode)?currentMode-7:currentMode-1));
	erase();
	move(MSG(3),72);
	attron(COLOR_PAIR(2));
	addstr("[\\]");
	attroff(COLOR_PAIR(2));
	if(sub)
		attron(COLOR_PAIR(1));
	addstr("SUB");
	if(sub)
		attroff(COLOR_PAIR(1));
	for(i=0;i<sizeof(items)/sizeof(items[0]);i++){
		if(!(items[i].top&curM))
			continue;
		if(prev!=items[i].group) {
			move(items[i].group,0);
			attron(COLOR_PAIR(3));
			switch(items[i].group){
				case VIBRATO:
					addstr("Vibrato ");
					break;
				case PERCUSSION:
					addstr("Percussion ");
					break;
				case AC:
					addstr("Accompaniment ");
					break;
				case ABC:
					addstr("AutoBaseCode ");
					break;
				case MOC:
					addstr("MelodyOnCode ");
					break;
				case REVERB:
					addstr("Reverb ");
					break;
				case TREMOLO:
					addstr("Tremolo ");
					break;
				case SYMPHONIC:
					addstr("Symphonic ");
					break;
				case DELAY:
					addstr("Delay ");
					break;
				case FLANGER:
					addstr("Flanger ");
					break;
				case DISTORTION:
					addstr("Distortion ");
					break;
			}
			attroff(COLOR_PAIR(3));
		} else
			addch(' ');
		prev=items[i].group;
		if(items[i].key!=currentKey&&!(KEY_F(1)<=items[i].key&&items[i].key<=KEY_F(12)&&items[i].key-KEY_F0==currentMode))
			attron(COLOR_PAIR(2));
		else
			attron(COLOR_PAIR(1));
		addch('[');
		if(0x21<=items[i].key&&items[i].key<=0x7E){
			addch(items[i].key);
		}else if(KEY_F(1)<=items[i].key&&items[i].key<=KEY_F(12)){
			addch('F');
			printw("%d",items[i].key-KEY_F0);
		}
		addch(']');
		if(items[i].key!=currentKey&&!(KEY_F(1)<=items[i].key&&items[i].key<=KEY_F(12)&&items[i].key-KEY_F0==currentMode))
			attroff(COLOR_PAIR(2));
		addstr(items[i].name);
		if(items[i].key==currentKey||(KEY_F(1)<=items[i].key&&items[i].key<=KEY_F(12)&&items[i].key-KEY_F0==currentMode))
			attroff(COLOR_PAIR(1));
		if(items[i].key==currentKey){
			ready=1;
			int x,y;
			getyx(stdscr,y,x);
			if(items[i].max<currentValue)
				currentValue=items[i].max;
			if(currentValue<items[i].min)
				currentValue=items[i].min;
			move(CURRENTVALUE,39);
			printw("%d",currentValue);
			move(CURRENTVALUE,0);
			printw("%d",items[i].min);
			move(CURRENTVALUE,77);
			printw("%d",items[i].max);
			if(items[i].minS!=NULL){
				move(MSG(1),0);
				addstr(items[i].minS);
			}
			if(items[i].maxS!=NULL){
				move(MSG(1),80-strlen(items[i].maxS));
				addstr(items[i].maxS);
			}
			data1[ADDR(3)]=data2[ADDR(3)]=items[i].Laddr;
			data1[DATA]=data2[DATA]=currentValue;
			if(1<=currentMode&&currentMode<=5){
				data1[ADDR(1)]=data2[ADDR(1)]=(sub&&(items[i].group==REVERB||items[i].group==TREMOLO||items[i].group==SYMPHONIC
												||items[i].group==DELAY||items[i].group==FLANGER||items[i].group==DISTORTION||items[i].Laddr==0x3F))?0x01:0x00;
				data1[ADDR(2)]=data2[ADDR(2)]=currentMode-1;
			}else if(6<=currentMode&&currentMode<=7){
				data1[ADDR(1)]=data2[ADDR(1)]=(sub&&(items[i].group==REVERB||items[i].group==TREMOLO||items[i].group==SYMPHONIC
												||items[i].group==DELAY||items[i].group==FLANGER||items[i].group==DISTORTION||items[i].Laddr==0x3F))?0x01:0x00;
				data1[ADDR(2)]=data2[ADDR(2)]=currentMode;
			}else if(9<=currentMode&&currentMode<=11){
				data1[ADDR(1)]=data2[ADDR(1)]=0x02;
				data1[ADDR(2)]=data2[ADDR(2)]=currentMode-9;
			}else if(currentMode==8){
				data1[ADDR(1)]=data2[ADDR(1)]=0x03;
				if(items[i].group==AC){
					data1[ADDR(2)]=data2[ADDR(2)]=0x02;
				}else if(items[i].group==ABC){
					data1[ADDR(2)]=data2[ADDR(2)]=0x03;
				}else if(items[i].group==MOC){
					data1[ADDR(2)]=data2[ADDR(2)]=0x04;
				}else{
					data1[ADDR(2)]=data2[ADDR(2)]=0x00;
				}
			}else if(currentMode==12){
				data1[ADDR(1)]=data2[ADDR(1)]=0x04;
				data1[ADDR(2)]=data2[ADDR(2)]=0x00;
			}else if(currentMode==20){
				data1[ADDR(1)]=data2[ADDR(1)]=0x03;
				data1[ADDR(2)]=data2[ADDR(2)]=0x00;
				ready=4;
			}else{
				data1[ADDR(1)]=data2[ADDR(1)]=0x00;
				data1[ADDR(2)]=data2[ADDR(2)]=currentMode-13;
				data2[ADDR(3)]=data1[DATA]=0x01;
				data1[ADDR(3)]=0x10;
				data2[DATA]=items[i].Laddr;
				data2[DATA+1]=currentValue;
				ready=3;
				if(currentMode==17){
					data2[ADDR(3)]=data1[DATA]=0x00;
				}else if(18<=currentMode&&currentMode<=19){
					data1[ADDR(2)]++;
					data2[ADDR(2)]++;
					data2[ADDR(3)]=data1[DATA]=0x00;
				}
			}
			move(y,x);
		}
	}
	move(MSG(2),0);
	printw("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X"
		,data1[0],data1[1],data1[2],data1[3],data1[4],data1[5]
		,data1[6],data1[7],data1[8],data1[9]);
	move(MSG(3),0);
	printw("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X"
		,data2[0],data2[1],data2[2],data2[3],data2[4],data2[5]
		,data2[6],data2[7],data2[8],data2[9],data2[10]);
	refresh();
}

void send(){
	switch(ready){
	case 1:
		snd_rawmidi_write(dev,data1,10);
		break;
	case 2:
		snd_rawmidi_write(dev,data2,11);
		break;
	case 3:
		snd_rawmidi_write(dev,data1,10);
		snd_rawmidi_write(dev,data2,11);
		break;
	case 4:
		snd_rawmidi_write(dev,data1,10);
		data1[ADDR(3)]=0x10;
		data1[DATA]=0x01;
		snd_rawmidi_write(dev,data1,10);
		break;
	}
}

int main(int argc,char *argv[]){
	int c;
	if(argc<2)
		return 1;
	c = snd_rawmidi_open(NULL, &dev, argv[1], SND_RAWMIDI_SYNC);
	if(c<0)
		return 1;
	initscr();
	if(has_colors()==FALSE)
		return 1;
	start_color();
	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,COLOR_YELLOW,COLOR_BLACK);
	init_pair(3,COLOR_GREEN,COLOR_BLACK);
	noecho();
	raw();
	keypad(stdscr, TRUE);
	draw();
	int curM,i;
	while((c = getch()) != 27){
		if(c=='\\'){
			sub=!sub;
			draw();
			continue;
		}
		if('a'<=c&&c<='z')
			c-=0x20;
		curM=1<<((13<=currentMode&&currentMode<=19)?12:((20<=currentMode)?currentMode-7:currentMode-1));
		if(c==' '){
			//TODO
		}else if(c=='\n'){
			send();
		}else if(c==KEY_UP||c==KEY_RIGHT){
			currentValue++;
		}else if(c==KEY_DOWN||c==KEY_LEFT){
			currentValue--;
		}
		for(i=0;i<sizeof(items)/sizeof(items[0]);i++){
			if(!(items[i].top&curM))
				continue;
			if(items[i].key==c){
				if(items[i].min==-2){
					currentMode+=items[i].max;
				}else if(items[i].min==-1){
					currentMode=items[i].max;
				}else if(items[i].min>=0){
					currentKey=c;
				}
				break;
			}
		}
		draw();
	}
	endwin();
	snd_rawmidi_close(dev);
	dev=NULL;
	return 0;
}