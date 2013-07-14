#include <stdio.h>
#include <malloc.h>
const char cst[] = {0xF0,0x43,0x70,0x78,0x20,0xF7};
int main(int argc,char *argv[]){
	if(argc!=3)
		return -1;
	
	FILE *fpr,*fpw;
	unsigned char *data,t;
	unsigned int i,j;
	
	if((fpr = fopen(argv[1],"rb")) == NULL)
		return -1;
	if((fpw = fopen(argv[2],"wb")) == NULL)
		return -1;
	
	//fwrite(cst,1,6,fpw);
	
	fseek(fpr,0,SEEK_END);
	j = ftell(fpr);
	fseek(fpr,0,SEEK_SET);
	data = malloc(j);
	if(data == NULL){
		puts("can't alloc memory.");
		return -1;
	}
	j = fread(data,1,j,fpr);
	fclose(fpr);
	
	fwrite(data,1,8,fpw);
	i=8;
	
	for(;i<j;i++){
		if(data[i]==0xF7){
			fwrite(data+i,1,1,fpw);
			break;
		} else if(0x40<=data[i]&&data[i]<=0x7F) {
			fwrite(data+i,1,1,fpw);
			t = 0x50;
			fwrite(&t,1,1,fpw);
		} else if(0x80<=data[i]&&data[i]<=0xBF) {
			data[i] -= 0x40;
			fwrite(data+i,1,1,fpw);
			t = 0x60;
			fwrite(&t,1,1,fpw);
		} else if(0xC0<=data[i]&&data[i]<=0xFF) {
			data[i] -= 0x80;
			fwrite(data+i,1,1,fpw);
			t = 0x70;
			fwrite(&t,1,1,fpw);
		} else {
			fwrite(data+i,1,1,fpw);
		}
	}
	fclose(fpw);
	return 0;
}