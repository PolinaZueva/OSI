#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define ERROR -1

int main() {
    char* envVar = getenv("MY_VAR");
    if (envVar == NULL) {
        printf("Ошибка: Переменная MY_VAR не найдена\n");
        return EXIT_FAILURE;
    } 
    printf("Начальное значение MY_VAR: %s\n", envVar);

    int setenvResult = setenv("MY_VAR", "newVar", 1);
    if (setenvResult == ERROR) {
        perror("Ошибка в sevenv при изменении MY_VAR");
        return EXIT_FAILURE;
    }

    envVar = getenv("MY_VAR");
    if (envVar == NULL) {
        printf("Ошибка: Переменная MY_VAR не найдена после изменения\n");
        return EXIT_FAILURE;
    } 
    printf("Новое значение MY_VAR: %s\n", envVar);         
    return EXIT_SUCCESS;
}