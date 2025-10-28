#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define RETURN_CODE 0
#define ERROR -1

void *mythread(void *arg) {
	while (true) {
        printf("mythread [%d %d %d]: sends a message...\n", getpid(), getppid(), gettid());
    }
    return NULL;
}

int main() {
	pthread_t tid;
	int err;
	void *thread_ret_result;

	printf("main [%d %d %d]: Hello from main\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err != RETURN_CODE) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return ERROR;
	}
    
    err = pthread_cancel(tid);
    if (err != RETURN_CODE) {
        printf("main: pthread_cancel() failed: %s\n", strerror(err));
		return ERROR;
    }	
    
    err = pthread_join(tid, &thread_ret_result);
    if (err != RETURN_CODE) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
		return ERROR;
    }	

    if (thread_ret_result == PTHREAD_CANCELED) {
        printf("main [%d %d %d]: thread was canceled\n", getpid(), getppid(), gettid());
    }    
	return RETURN_CODE;
}