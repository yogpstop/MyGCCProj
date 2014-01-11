int do() {
	unsigned int x,y,s=0,d=0;
	const unsigned int wda=str->bmih.biWidth*3;
	unsigned int r,g,b;
	for(y=0;y<360;y++) {
		for(x=0;x<640;x++) {
			r=str->bitmap[s];
			r+=str->bitmap[(s++)+wda];
			g=str->bitmap[s];
			g+=str->bitmap[(s++)+wda];
			b=str->bitmap[s];
			b+=str->bitmap[(s++)+wda];
			r+=str->bitmap[s];
			r+=str->bitmap[(s++)+wda];
			g+=str->bitmap[s];
			g+=str->bitmap[(s++)+wda];
			b+=str->bitmap[s];
			b+=str->bitmap[(s++)+wda];
			out[d++]=(unsigned char)(r/4);
			out[d++]=(unsigned char)(g/4);
			out[d++]=(unsigned char)(b/4);
		}
		s+=wda;
	}
}