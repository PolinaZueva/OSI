#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define RETURN_CODE 0
#define NUM_THREADS 5

void *mythread(void *arg) {
	int thread_num = *(int *)arg;
	printf("mythread %d: [%d %d %d]\n", thread_num, getpid(), getppid(), gettid());
	return NULL;
}

int main() {
	pthread_t tids[NUM_THREADS];  //id потоков	
	int thread_nums[NUM_THREADS];  //номера потоков
	int err;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	for (int i = 0; i < NUM_THREADS; i++) {
		thread_nums[i] = i;
		err = pthread_create(&tids[i], NULL, mythread, &thread_nums[i]);
		if (err != RETURN_CODE) {
	    	printf("main: pthread_create() failed for thread %d: %s\n", i, strerror(err));
			return -1;
		}
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		err = pthread_join(tids[i], NULL);
    	if (err != RETURN_CODE) {
        	printf("main: pthread_join() failed for thread %d: %s\n", i, strerror(err));
			return -1;
   		}
	}    

	return 0;
}