#include "helpers.h"

int reportError(char* action, char* path) {
    char errorMsg[ERROR_MSG_SIZE];
    snprintf(errorMsg, sizeof(errorMsg), "Failed to %s %s", action, path);
    perror(errorMsg);
    return ERROR_CODE;
}

void reverseBuffer(char* buffer, ssize_t size) {
    ssize_t sizeBuffer = size / 2;
    for (ssize_t i = 0; i < sizeBuffer; i++) {
        char tempBuffer = buffer[i];
        buffer[i] = buffer[size - i - 1];
        buffer[size - i - 1] = tempBuffer;
    }
}