NAME=niconama_alert

windows:
	rm -f ../dist/$(NAME).exe
	gcc -Wall -O3 -s -o ../dist/$(NAME).exe windows.c
linux:
	rm -f ../dist/$(NAME)
	gcc -Wall -O3 -s -o ../dist/$(NAME) linux.c