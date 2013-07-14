name = x264_output
all:
	rm -f $(name).auo
	gcc -Wall -O3 --exec-charset=cp932 -c -o $(name).o $(name).c
	windres --language=0411 --input-format=rc --output-format=coff -o $(name).res $(name).rc
	gcc -s -shared -o ../dist/$(name).auo $(name).o $(name).res $(name).def -lcomdlg32
	rm -f $(name).res $(name).o