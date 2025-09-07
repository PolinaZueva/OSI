#ifndef DIR_FUNCS_H
#define DIR_FUNCS_H

#include "helpers.h"
#include "file_funcs.h"
#include <sys/stat.h>
#include <dirent.h>

#define DIRECTORY_ACCESS (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)  //rwxr-xr-x

int buildPath(char* path, int maxNameLen, char* dir, char* name);
int createDirectory(char* path);
int reverseName(char* reversedName, size_t maxLen, char* name);
int reverseDirectory(char* srcPath, char* destPath);
int handleFileOrDirectory(struct stat* sb, char* srcPath, char* destPath, DIR* dir);
int processDirectoryEntry(struct dirent *entry, char* srcDir, char* destDir, DIR *dir);
int traverseAndWorkDirectory(char* srcPath, char* destPath);

#endif