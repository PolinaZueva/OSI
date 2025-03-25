#ifndef DIR_FUNCS_H
#define DIR_FUNCS_H

#include "helpers.h"
#include "file_funcs.h"
#include <sys/stat.h>
#include <dirent.h>

#define DIRECTORY_ACCESS (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)  //rwxr-xr-x

int buildPath(char* path, int maxNameLen, char* dir, char* name);
int createDirectory(char* path);
char* createReversedName(char* name);
int processFile(char* srcPath, char* destPath, char* reversedName, DIR* dir);
int processDirectory(char* srcPath, char* destPath, char* reversedName, DIR* dir);
int processEntryType(struct stat* sb, char* srcPath, char* destPath, char* reversedName, DIR* dir);
int processEntry(struct dirent *entry, char* srcDir, char* destDir, DIR *dir);
int workWithDirectory(char* srcPath, char* destPath);

#endif