#include "dir_funcs.h"

int buildPath(char* path, int maxNameLen, char* dir, char* name) {
    int lenPath = snprintf(path, maxNameLen, "%s/%s", dir, name);
    if (lenPath >= maxNameLen) {
        fprintf(stderr, "Path is too long: %s/%s\n", dir, name);
        return ERROR_CODE;
    }  
    return 0;
}

int createDirectory(char* path) {
    int mkdirResult = mkdir(path, DIRECTORY_ACCESS);
    if (mkdirResult == ERROR_CODE && errno != EEXIST) {  
        return reportError("create directory", path); 
    }
    return 0;
}

int reverseName(char* reversedName, size_t maxLen, char* name) {
    size_t lenName = strlen(name);
    if (lenName >= maxLen) {
        fprintf(stderr, "Name is too long: %s\n", name);
        return ERROR_CODE;
    }
    strcpy(reversedName, name);
    reverseBuffer(reversedName, lenName);
    return 0;
}

int reverseDirectory(char* srcPath, char* destPath) {
    int mkdirResult = createDirectory(destPath);
    if (mkdirResult == ERROR_CODE) {
        return ERROR_CODE;
    }

    int recursiveResult = traverseAndWorkDirectory(srcPath, destPath);
    if (recursiveResult == ERROR_CODE) {
        return ERROR_CODE;
    }
    return 0;
}

int handleFileOrDirectory(struct stat* pathInfo, char* srcPath, char* destPath, DIR* dir) {
    int processResult = 0;
    if (S_ISREG(pathInfo->st_mode)) {
        processResult = checkReversedFile(srcPath, destPath);
    }

    if (S_ISDIR(pathInfo->st_mode)) {
        processResult = reverseDirectory(srcPath, destPath);
    }

    if (processResult == ERROR_CODE) {
        closedir(dir);
        return ERROR_CODE;
    }
    return 0;
}

int processDirectoryEntry(struct dirent *entry, char* srcDir, char* destDir, DIR *dir) {
    char srcPath[MAX_NAME_LEN];        
    int srcResult = buildPath(srcPath, MAX_NAME_LEN, srcDir, entry->d_name);
    if (srcResult == ERROR_CODE) { 
        closedir(dir);
        return reportError("build source path", srcPath);
    }        
        
    char reversedName[MAX_NAME_LEN];
    int reversedResult = reverseName(reversedName, MAX_NAME_LEN, entry->d_name);
    if (reversedResult == ERROR_CODE) {
        closedir(dir);
        return ERROR_CODE;
    }
         
    char destPath[MAX_NAME_LEN];
    int destResult = buildPath(destPath, MAX_NAME_LEN, destDir, reversedName);
    if (destResult == ERROR_CODE) { 
        closedir(dir);
        return reportError("build destination path", destPath);
    }
            
    struct stat pathInfo; 
    int statResult = lstat(srcPath, &pathInfo);
    if (statResult == ERROR_CODE) { 
        closedir(dir);
        return reportError("get file info about", srcPath);       
    }          
     
    int processResult = handleFileOrDirectory(&pathInfo, srcPath, destPath, dir);
    if (processResult == ERROR_CODE) {
        return ERROR_CODE;
    }
    return 0;
}

int traverseAndWorkDirectory(char* srcPath, char* destPath) {
    DIR *dir = opendir(srcPath);  
    if (dir == NULL) {
        return reportError("open directory", srcPath);
    }

    struct dirent *entry;
    char* curDir = ".";
    char* parentDir = "..";
    while ((entry = readdir(dir)) != NULL) {        
        if (strcmp(entry->d_name, curDir) == 0 || strcmp(entry->d_name, parentDir) == 0) {
            continue;
        }

        int processResult = processDirectoryEntry(entry, srcPath, destPath, dir);  //обрабатываем файлы и каталоги
        if (processResult == ERROR_CODE) {
            return ERROR_CODE;
        }        
    }

    int closeDirResult = closedir(dir);
    if (closeDirResult == ERROR_CODE) {
        return reportError("close directory", srcPath);        
    }    
    return 0;
}