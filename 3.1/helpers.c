#include "helpers.h"

int reportError(char* action, char* path) {
    char errorMsg[ERROR_MSG_SIZE];
    snprintf(errorMsg, sizeof(errorMsg), "Failed to %s %s", action, path);
    perror(errorMsg);
    return ERROR_CODE;
}

void reverseString(char* str) {
    size_t lenStr = strlen(str);
    for (size_t i = 0; i < lenStr / 2; i++) {
        char tempStr = str[i];
        str[i] = str[lenStr - i - 1];
        str[lenStr - i - 1] = tempStr;
    }
}

void reverseBuffer(char* buffer, ssize_t size) {
    ssize_t sizeBuffer = size / 2;
    for (ssize_t i = 0; i < sizeBuffer; i++) {
        char tempBuffer = buffer[i];
        buffer[i] = buffer[size - i - 1];
        buffer[size - i - 1] = tempBuffer;
    }
}