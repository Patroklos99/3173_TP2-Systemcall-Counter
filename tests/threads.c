#include <stdlib.h>
#include <pthread.h>

int inf = 3173;

void * inc(void * t) {
    inf += * (int *) t;         
}

int main(int argc, char ** argv) {
	if (argc == 2) {
		char ** eptr;
		long nb_threads = strtol(argv[1], eptr, 10);
		pthread_t * threads = malloc(sizeof(pthread_t) * nb_threads);
		for (int i = 0; i < nb_threads; i++)
			pthread_create(&threads[i], NULL, inc, &i);
		for (int i = 0; i < nb_threads; i++)
			pthread_join(threads[i], NULL);
    		return nb_threads;
	}
	return 1;
}
