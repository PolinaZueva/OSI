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

void *mythread(void *arg) {
	printf("mythread [%d %d %d]: Hello from mythread\n", getpid(), getppid(), gettid());
	int *ret_num = malloc(sizeof(int));
	if (ret_num == NULL) {
		printf("mythread: malloc failed\n");
		return NULL;
	}
	*ret_num = 42;
	return (void*)ret_num;
}

int main() {
	pthread_t tid;
	int err;
	int *thread_ret_res;

	printf("main [%d %d %d]: Hello from main\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err != RETURN_CODE) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return ERROR;
	}
	
    err = pthread_join(tid, (void**)&thread_ret_res);
    if (err != RETURN_CODE) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
		return ERROR;
    }	
	printf("main [%d %d %d]: thread returned: %d\n", getpid(), getppid(), gettid(), *thread_ret_res);
	free(thread_ret_res);
	return 0;
}