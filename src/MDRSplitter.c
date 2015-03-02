#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>

int main(int argc,char *argv[]){
	if(argc!=2)
		return -1;
	
	FILE *fp;
	unsigned long ptr, ptr2;
	char *s, fn_cache[strlen(argv[1])+13];
	
	// ファイルの内容を全て変数に収める処理
	if((fp = fopen(argv[1], "rb")) == NULL)
		return -1;
	fseek(fp, 0, SEEK_END);
	ptr2 = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	s = malloc(ptr2);
	if(s == NULL)
		return -1;
	fread(s, 1, ptr2, fp);
	fclose(fp);
	
	ptr2=strlen(argv[1]);
	while(argv[1][--ptr2]!='.')
		if(argv[1][ptr2]=='\\'||argv[1][ptr2]=='/')
			break;
	argv[1][ptr2] = '/';
	argv[1][++ptr2] = 0;
	
#ifdef _WIN32
	mkdir(argv[1]);
#else
	mkdir(argv[1],S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif
	
	for(ptr=0x2600;ptr<0x3E00;ptr+=0x20){
		//ファイル名1文字目がNULL,ファイルサイズが0
		if(s[ptr]==0 || !(s[ptr+0x1C]||s[ptr+0x1D]||s[ptr+0x1E]||s[ptr+0x1F]))
			continue;
		
		//ファイル名の作成
		memset(fn_cache, 0, strlen(argv[1])+13);
		strcpy(fn_cache, argv[1]);
		ptr2 = ptr+0x08;
		while(s[--ptr2]==0x20);
		s[++ptr2] = 0;
		strcat(fn_cache, s+ptr);
		fn_cache[strlen(fn_cache)] = '.';
		memcpy(fn_cache+strlen(fn_cache), s+ptr+8, 3);
		
		//データを書き込む
		if((fp = fopen(fn_cache, "wb"))==NULL) {
			printf("Can't open %s",fn_cache);
			continue;
		}
		fwrite(s+0x3E00+((((unsigned char)s[ptr+0x1B])<<8)|((unsigned char)s[ptr+0x1A]))*0x200,
				1, (((unsigned char)s[ptr+0x1F])<<24)|(((unsigned char)s[ptr+0x1E])<<16)|(((unsigned char)s[ptr+0x1D])<<8)|((unsigned char)s[ptr+0x1C]), fp);
		fclose(fp);
	}
	free(s);
	return 0;
}
