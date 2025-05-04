#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define ERROR_CODE -1
#define VALUE_FOR_CHILD 0

int globalInitialVar = 1;

void childProcess(int* localInitialVar) {
    printf("Child: PID: %d\n", getpid());
    printf("Child: Родительский PID: %d\n", getppid());
    printf("Child: Содержимое локальной переменной: %d, адрес локальной переменной: %p\n", *localInitialVar, (void*)localInitialVar);
    printf("Child: Содержимое глобальной переменной: %d, адрес глобальной переменной: %p\n", globalInitialVar, (void*)&globalInitialVar);
    *localInitialVar = 3;
    globalInitialVar = 4;
    printf("Child: Содержимое локальной переменной после изменений: %d\n", *localInitialVar);
    printf("Child: Содержимое глобальной переменной после изменений: %d\n\n", globalInitialVar); 
    getc(stdin); 
    _exit(5);        
}

void parentProcess(int* localInitialVar) {
    sleep(20);
    int status;
    pid_t resultWait = wait(&status);
    if (resultWait == ERROR_CODE) {
        perror("Failed to wait\n");
        exit(EXIT_FAILURE);
    }
    printf("Parent: Содержимое локальной переменной после изменений: %d\n", *localInitialVar);
    printf("Parent: Содержимое глобальной переменной после изменений: %d\n", globalInitialVar);

    int endWithCode = WIFEXITED(status);
    int endWithSignal = WIFSIGNALED(status);
    if (endWithCode) {  
        int exitCode = WEXITSTATUS(status);
        printf("Parent: Дочерний процесс завершился с кодом: %d\n", exitCode);
    } 
    if (endWithSignal) {
        int signal = WTERMSIG(status);
        printf("Parent: Дочерний процесс завершился с сигналом: %d\n", signal);
    }    
}

int main() {
    int localInitialVar = 2;
    printf("Parent: Содержимое локальной переменной: %d, адрес локальной переменной: %p\n", localInitialVar, (void*)&localInitialVar);
    printf("Parent: Содержимое глобальной переменной: %d, адрес глобальной переменной: %p\n", globalInitialVar, (void*)&globalInitialVar);
    printf("Parent: PID: %d\n\n", getpid());

    pid_t process = fork();  //создаю дочерний процесс
    if (process == ERROR_CODE) {
        perror("Failed to fork");
        return EXIT_FAILURE;
    }

    if (process == VALUE_FOR_CHILD) {
        childProcess(&localInitialVar);
    } 

    parentProcess(&localInitialVar);
    return EXIT_SUCCESS;
}