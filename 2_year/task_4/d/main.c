//создает и инициализирует локальную переменную и возвращает ее адрес

#include <stdio.h>

int* getAddressLocalVar() {
    int localVar = 1;
    printf("localVar внутри функции: %d\n", localVar);
    printf("Aдрес локальной переменной в функции getAddressLocalVar: %p\n", (void*)&localVar);
    return &localVar;
}


int main() {
    int* addresslocalVar = getAddressLocalVar();
    printf("Aдрес, возвращенный в функцию main: %p\n", (void*)&addresslocalVar);
    printf("Пытаемся достать значение локальной переменной: %d\n", *addresslocalVar);
    return 0;
}