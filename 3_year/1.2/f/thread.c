#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define RETURN_CODE 0
#define ERROR -1

void *mythread(void *arg) {
	printf("mythread [%d %d %d]: Hello from mythread\n", getpid(), getppid(), gettid());
	return NULL;
}

int main() {
	pthread_t tid;
	int err;    
	int count_thread = 0;
    pthread_attr_t attr;

	printf("main [%d %d %d]: Hello from main\n", getpid(), getppid(), gettid());

    err = pthread_attr_init(&attr);
    if (err != RETURN_CODE) {
        printf("main: pthread_attr_init() failed: %s\n", strerror(err));
		return ERROR;
    }	

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err != RETURN_CODE) {
        printf("main: pthread_attr_setdetachstate() failed: %s\n", strerror(err));
		return ERROR;
    }	

	while(1) {
		err = pthread_create(&tid, &attr, mythread, NULL);
		if (err != RETURN_CODE) {
	    	printf("main: pthread_create() failed: %s\n", strerror(err));
			break;
		}
		count_thread++;
	}

    err = pthread_attr_destroy(&attr);
    if (err != RETURN_CODE) {
        printf("main: pthread_attr_destroy() failed: %s\n", strerror(err));
		return ERROR;
    }	

	printf("main [%d %d %d]: created %d threads\n", getpid(), getppid(), gettid(), count_thread);
	return 0;
}