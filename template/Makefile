name = win_gui
all:
	rm -f ../dist/$(name).exe
	gcc -Wall -c -O3 -o $(name).o $(name).c
	windres --input-format=rc --output-format=coff -o $(name).res $(name).rc
	gcc -s -mwindows -o ../dist/$(name).exe $(name).o $(name).res
	rm -f $(name).o $(name).res

