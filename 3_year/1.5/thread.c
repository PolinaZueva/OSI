#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#define RETURN_CODE 0
#define ERROR -1
#define SLEEP_TIME 10

void *thread_block_signals(void *arg) {
    int err;
    sigset_t all_signals;
    sigfillset(&all_signals);
    err = pthread_sigmask(SIG_BLOCK, &all_signals, NULL);
    if (err != RETURN_CODE) {
	    printf("thread_block_signals: pthread_sigmask() failed: %s\n", strerror(err));
		return (void *)ERROR;
	}

    printf("thread_block_signals [%d]: blocking all signals...\n", gettid());
	while (true) {
        sleep(SLEEP_TIME);        
    }
    return NULL;
}

void sigint_handler(int sig) {
    if (sig != SIGINT)
        return;
    char message[] = "thread_signal_handler: received SIGINT\n";
    write(STDOUT_FILENO, message, sizeof(message) - 1);
}

void *thread_signal_handler(void *arg) {
    int err;
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    err = sigaction(SIGINT, &sa, NULL);
    if (err != RETURN_CODE) {
	    printf("thread_signal_handler: sigaction() failed: %s\n", strerror(err));
		return (void *)ERROR;
	}

    sigset_t unblock_sigint;
    sigemptyset(&unblock_sigint);
    err = sigaddset(&unblock_sigint, SIGINT);
	if (err != RETURN_CODE) {
	    printf("thread_signal_handler: sigaddset() failed: %s\n", strerror(err));
		return (void *)ERROR;
	}
    err = pthread_sigmask(SIG_UNBLOCK, &unblock_sigint, NULL);
    if (err != RETURN_CODE) {
	    printf("thread_signal_handler: pthread_sigmask() failed: %s\n", strerror(err));
		return (void *)ERROR;
	}

    printf("thread_signal_handler [%d]: waiting for SIGINT...\n", gettid());   
	while (true) {          
        sleep(SLEEP_TIME);           
    }
    return NULL;
}

void *thread_sigwait(void *arg) {
    int err;
    sigset_t set_wait;
    int sig;
    sigemptyset(&set_wait);
    err = sigaddset(&set_wait, SIGQUIT);
	if (err != RETURN_CODE) {
	    printf("thread_sigwait: sigaddset() failed: %s\n", strerror(err));
		return (void *)ERROR;
	}
    printf("thread_sigwait [%d]: waiting for SIGQUIT...\n", gettid());    
    err = sigwait(&set_wait, &sig);
    if (err != RETURN_CODE) {
        printf("thread_sigwait: sigwait() failed: %s\n", strerror(err));
        return (void *)ERROR;
    }
    printf("thread_sigwait [%d]: received SIGQUIT\n", gettid());
    return NULL;
}

int main() {
	pthread_t tid1, tid2, tid3;
	int err;
    sigset_t set_signals;    
	printf("main [%d %d %d]: Hello from main\n", getpid(), getppid(), gettid());
    printf("kill -SIGKILL %d\nkill -SIGINT %d\nkill -SIGQUIT %d\n", getpid(), getpid(), getpid());

    sigemptyset(&set_signals);
    err = sigaddset(&set_signals, SIGINT);
	if (err != RETURN_CODE) {
	    printf("main: sigaddset() failed: %s\n", strerror(err));
		return ERROR;
	}
    err = sigaddset(&set_signals, SIGQUIT);
	if (err != RETURN_CODE) {
	    printf("main: sigaddset() failed: %s\n", strerror(err));
		return ERROR;
	}
    err = pthread_sigmask(SIG_BLOCK, &set_signals, NULL);
    if (err != RETURN_CODE) {
	    printf("main: pthread_sigmask() failed: %s\n", strerror(err));
		return ERROR;
	}

	err = pthread_create(&tid1, NULL, thread_block_signals, NULL);
	if (err != RETURN_CODE) {
	    printf("main: pthread_create(tid1) failed: %s\n", strerror(err));
		return ERROR;
	}
    err = pthread_create(&tid2, NULL, thread_signal_handler, NULL);
	if (err != RETURN_CODE) {
	    printf("main: pthread_create(tid2) failed: %s\n", strerror(err));
		return ERROR;
	}
    err = pthread_create(&tid3, NULL, thread_sigwait, NULL);
	if (err != RETURN_CODE) {
	    printf("main: pthread_create(tid3) failed: %s\n", strerror(err));
		return ERROR;
	}

    err = pthread_join(tid1, NULL);
    if (err != RETURN_CODE) {
        printf("main: pthread_join(tid1) failed: %s\n", strerror(err));
		return ERROR;
    }	
    err = pthread_join(tid2, NULL);
    if (err != RETURN_CODE) {
        printf("main: pthread_join(tid2) failed: %s\n", strerror(err));
		return ERROR;
    }	
    err = pthread_join(tid3, NULL);
    if (err != RETURN_CODE) {
        printf("main: pthread_join(tid3) failed: %s\n", strerror(err));
		return ERROR;
    }	    
	return RETURN_CODE;
}