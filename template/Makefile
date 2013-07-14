name = win_gui
all:
	rm -f $(name).exe
	gcc -Wall -c -O3 -o $(name).o $(name).c
	windres --input-format=rc --output-format=coff -o $(name).res $(name).rc
	gcc -s -mwindows -o $(name).exe $(name).o $(name).res
	rm -f $(name).o $(name).res