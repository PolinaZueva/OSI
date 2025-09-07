#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void workWithBuffer() {
    char* buffer1 = malloc(100);
    if (buffer1 == NULL) {
        printf("Ошибка выделения на куче буфера 1");
        return;   
    }
    strcpy(buffer1, "hello world");
    printf("iii. Содержимое буфера 1: %s\n", buffer1);
    free(buffer1);
    printf("v. Содержимое буфера 1 после free: %s\n", buffer1);

    char* buffer2 = malloc(100);
    if (buffer2 == NULL) {
        printf("Ошибка выделения на куче буфера 2");
        return;   
    }
    strcpy(buffer2, "hello world");
    printf("viii. Содержимое буфера 2: %s\n", buffer2);
    char* middleOfBuffer2 = buffer2 + 50;   
    free(middleOfBuffer2);
    printf("Содержимое буфера 2 после освобождения middleOfBuffer2: %s\n", buffer2);
}

int main() {
    workWithBuffer();
    return 0;
}