#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define RETURN_CODE 0
#define NUM_THREADS 5

pthread_t created_tids[NUM_THREADS];

void *mythread(void *arg) {
	int thread_num = *(int *)arg;
    printf("mythread %d: [%d %d %d %ld]\n", thread_num, getpid(), getppid(), gettid(), pthread_self());

    if (pthread_equal(pthread_self(), created_tids[thread_num]) != RETURN_CODE) {
        printf("thread IDs match\n");
    } else {
        printf("thread IDs don't match\n");
    }

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
		created_tids[i] = tids[i];
		if (err != RETURN_CODE) {
	    	printf("main: pthread_create() failed for thread %d: %s\n", i, strerror(err));
			return -1;
		}
		printf("\npthread_create() stores the first element for a thread %d: %ld\n", i, tids[i]);
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