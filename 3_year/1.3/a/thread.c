#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define RETURN_CODE 0
#define ERROR -1

typedef struct {
    int number;
    char *message;
} mythread_struct;

void *mythread(void *arg) {
    mythread_struct* data = (mythread_struct*)arg;
	printf("mythread [%d %d %d]: Hello from mythread that receive number [%d], message [%s]\n", 
            getpid(), getppid(), gettid(), data->number, data->message);
	return NULL;
}

int main() {
	pthread_t tid;
	int err;
    mythread_struct data; 
    
	printf("main [%d %d %d]: Hello from main\n", getpid(), getppid(), gettid());

    data.number = 1;
    data.message = "hello from joinable";

	err = pthread_create(&tid, NULL, mythread, &data);
	if (err != RETURN_CODE) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return ERROR;
	}

    err = pthread_join(tid, NULL);
    if (err != RETURN_CODE) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
		return ERROR;
    }
	return 0;
}