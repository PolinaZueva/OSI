#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_NAME_LEN 256
#define BUFFER_SIZE 2048
#define ERROR_MSG_SIZE 512
#define ERROR_CODE -1

int reportError(char* action, char* path);
void reverseBuffer(char* buffer, ssize_t size);

#endif 