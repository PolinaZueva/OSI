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
    free(data->message);        
    free(data);    
	return NULL;
}

int main() {
	pthread_t tid;
	int err;
    pthread_attr_t attr;

    printf("main [%d %d %d]: Hello from main\n", getpid(), getppid(), gettid());

    mythread_struct* data = malloc(sizeof(mythread_struct));
    if (data == NULL) {
		printf("main: malloc failed\n");
		return ERROR;
	}
    data->number = 2;
    char* hello = "hello from detached";
    data->message = malloc(strlen(hello) + 1);
    if (data->message == NULL) {
        printf("main: malloc failed\n");
        free(data);
		return ERROR;
    }
    strcpy(data->message, hello);

    err = pthread_attr_init(&attr);
    if (err != RETURN_CODE) {
        printf("main: pthread_attr_init() failed: %s\n", strerror(err));
        free(data->message);
        free(data);        
		return ERROR;
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err != RETURN_CODE) {
        printf("main: pthread_attr_setdetachstate() failed: %s\n", strerror(err));
        free(data->message);
        free(data);
        pthread_attr_destroy(&attr);
		return ERROR;
    }	

	err = pthread_create(&tid, &attr, mythread, data);
	if (err != RETURN_CODE) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
        pthread_attr_destroy(&attr);
        free(data->message);
        free(data);
		return ERROR;
	}

    err = pthread_attr_destroy(&attr);
    if (err != RETURN_CODE) {
        printf("main: pthread_attr_destroy() failed: %s\n", strerror(err));
    }
    pthread_exit(NULL);  //завершаем main и поток продолжит свою работу
}