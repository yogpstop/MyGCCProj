NAME=MIDI_IN

windows:
	rm -f $(NAME).exe
	gcc -Wall -O3 -s -o ../dist/$(NAME).exe windows.c -lwinmm
linux:
	rm -f $(NAME)
	gcc -Wall -O3 -s -o ../dist/$(NAME) linux.c -lasound