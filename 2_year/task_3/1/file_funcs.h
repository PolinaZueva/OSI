#ifndef FILE_FUNCS_H
#define FILE_FUNCS_H

#include "helpers.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_ACCESS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)  //rw-rw-rw-

int openFiles(int* fdSrc, char* srcPath, int* fdDest, char* destPath);
int closeFiles(int fdSrc, char* srcPath, int fdDest, char* destPath);
int copyAndReverseFileContent(int fdSrc, char* srcPath, int fdDest, char* destPath);
int reverseFile(char* srcPath, char* destPath);
int checkReversedFile(char* srcPath, char* destPath);

#endif