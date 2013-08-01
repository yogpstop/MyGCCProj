NAME=niconama

windows:
	rm -f ../dist/$(NAME).exe
	gcc -Wall -O3 -s -o ../dist/$(NAME).exe windows/*.c
unix:
	rm -f ../dist/$(NAME)
	gcc -Wall -O3 -s -o ../dist/$(NAME) un/*.c -lpthread -lsqlite3