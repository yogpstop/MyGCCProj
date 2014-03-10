#include <pthread.h>
void listing(char **);
void list_shuffle();
void console();
void *play_thread(void *);
void *buffer_thread(void *);
int main(int argc, char **argv) {
	listing(argv+1);
	list_shuffle();
	pthread_t play, buffer;
	pthread_create(&buffer, NULL, &buffer_thread, NULL);
	pthread_create(&play, NULL, &play_thread, NULL);
	console();
	pthread_join(buffer, NULL);
	pthread_join(play, NULL);
	return 0;
}
