#include <stdio.h>
unsigned int min(unsigned int a, unsigned int b){
	if (a < b)
		return a;
	else
		return b;
}
const unsigned char vendor[] = {'v','e','n','d','o','r','='};
int vorbis_comment(FILE *fp, char *opf){
	FILE *fpw;
	unsigned char temp[1024];
	unsigned int  i, j, len, size, cont;
	
	if ((fpw = fopen(opf,"ab")) == NULL) {
		puts("Cannot open output_file.");
		return -1;
	}
	fread(&size,1,4,fp);
	fwrite(vendor,1,7,fpw);
	for(i = 0;i < size; i += len) {
		len = fread(temp,1,min(size,1024),fp);
		fwrite(temp,1,len,fpw);
	}
	putc('\n',fpw);
	
	fread(&cont,1,4,fp);
	for(j = 0; j < cont; j++){
		fread(&size,1,4,fp);
		for(i = 0; i < size; i += len){
			len = fread(temp,1,min(size,1024),fp);
			fwrite(temp,1,len,fpw);
		}
		putc('\n',fpw);
	}
	fclose(fpw);
	return 0;
}

int process(char *ipf, char *opf){
	FILE *fp;
	unsigned char str[4];
	unsigned int  size = 0;

	if ((fp = fopen(ipf,"rb")) == NULL) {
		puts("Cannot open input_file.");
		return -1;
	}
	fseek(fp,4,SEEK_SET);
	while(fread(str, 1, 4, fp) > 3) {
		size = (((unsigned int)str[1]) << 16) |
			(((unsigned int)str[2]) << 8) |
			((unsigned int)str[3]);
		if ((str[0] & 127) == 4)
			vorbis_comment(fp,opf);
		if (str[0] & 128)
			break;
		fseek(fp,size,SEEK_CUR);
	}
	
	fclose(fp);
	return 0;
}
int main(int argc, char *argv[]){
	if(argc != 3){
		puts("Usage: [exe pass] in_file out_file");
		return -1;
	}
	return process(argv[1],argv[2]);
}