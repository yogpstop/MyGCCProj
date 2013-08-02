NAME=niconama

windows:
	rm -f ../dist/$(NAME).exe
	gcc -Wall -O3 -s -o ../dist/$(NAME).exe windows/*.c
unix:
	rm -f ../dist/$(NAME)
	gcc -Wall -O3 -s `pkg-config --cflags gtk+-3.0` -o ../dist/$(NAME) un/*.c -lpthread -lsqlite3 `pkg-config --libs gtk+-3.0`