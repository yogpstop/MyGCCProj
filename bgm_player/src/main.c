#include <stdlib.h>
#include <pthread.h>
extern int force_exit_signal;
int listing(char **);
void list_shuffle();
void list_full_remove();
void clear_buffer();
void console();
void *play_thread(void *);
void *buffer_thread(void *);
int main(int argc, char **argv) {
	if(listing(argv+1)) force_exit_signal = 1;
	list_shuffle();
	pthread_t play, buffer;
	pthread_attr_t rt;
	struct sched_param p = {};
	p.sched_priority = sched_get_priority_max(SCHED_RR);
	pthread_attr_init(&rt);
	pthread_attr_setinheritsched(&rt, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&rt, SCHED_RR);
	pthread_attr_setschedparam(&rt, &p);
	pthread_create(&buffer, NULL, &buffer_thread, NULL);
	pthread_create(&play, &rt, &play_thread, NULL);
	console();
	pthread_join(buffer, NULL);
	pthread_join(play, NULL);
	list_full_remove();
	clear_buffer();
	return 0;
}