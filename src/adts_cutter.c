#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

//1frame=1024sample
//1sample=1/SamplingRate(sec)

int main(int argc, char *argv[]){
	if(argc!=5)
		return 1;
	
	FILE *fp;
	unsigned char *data;
	unsigned int ptr=0,j,start,end,cf,flen;
	
	start=atoi(argv[3]);
	end=atoi(argv[4]);
	
	if((fp=fopen(argv[1],"rb"))==NULL){
		puts("Can't open input file.");
		return 1;
	}
	fseek(fp,0,SEEK_END);
	j = ftell(fp);
	fseek(fp,0,SEEK_SET);
	data = malloc(j);
	if(data == NULL){
		puts("Can't alloc memory.");
		return 1;
	}
	j = fread(data,1,j,fp);
	fclose(fp);
	
	if((fp=fopen(argv[2],"wb"))==NULL){
		puts("Can't open output file.");
		return 1;
	}
	for(cf=0;cf<end;cf++){
		while(ptr<j&&(data[ptr]!=0xFF||(data[ptr+1]&0xF0)!=0xF0))
			ptr++;
		if(j<=ptr)
			break;
		flen=(data[ptr+3]&0x03)<<11;
		flen|=data[ptr+4]<<3;
		flen|=(data[ptr+5]&0xE0)>>5;
		if(start<=cf)
			fwrite(data+ptr,1,flen,fp);
		ptr+=flen;
	}
	free(data);
	fclose(fp);
	return 0;
}