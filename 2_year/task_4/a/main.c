#include <stdio.h>

int globalInitialVar = 4;
int globalNotInitialVar;
const int globalConstVar = 5;


void printAddress() {
    int localVar = 1;
    static int staticVar = 2;
    const int constVar = 3;

    printf("Адрес локальной переменной: %p\n", (void*)&localVar);
    printf("Адрес статической переменной: %p\n", (void*)&staticVar);
    printf("Адрес константы: %p\n", (void*)&constVar);
    printf("Адрес глобальной инциализированной переменной: %p\n", (void*)&globalInitialVar);
    printf("Адрес глобальной неинциализированной переменной: %p\n", (void*)&globalNotInitialVar);
    printf("Адрес глобальной константы: %p\n", (void*)&globalConstVar);
}

int main() {
    printAddress();
    return 0;
}