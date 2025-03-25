#include "dir_funcs.h"

int buildPath(char* path, int maxNameLen, char* dir, char* name) {
    int length = snprintf(path, maxNameLen, "%s/%s", dir, name);
    if (length < 0 || length >= maxNameLen) {
        fprintf(stderr, "Path too long: %s/%s\n", dir, name);
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

char* createReversedName(char* name) {
    char* reversedName = strdup(name);
    if (reversedName == NULL) {
        reportError("allocate memory for name", name);
        return NULL;
    }
    reverseString(reversedName);
    return reversedName;
}

int processFile(char* srcPath, char* destPath, char* reversedName, DIR* dir) {
    int reverseResult = reverseFile(srcPath, destPath);
    if (reverseResult == ERROR_CODE) {
        free(reversedName);
        closedir(dir);
        return ERROR_CODE;
    }
    return 0;
}

int processDirectory(char* srcPath, char* destPath, char* reversedName, DIR* dir) {
    int mkdirResult = createDirectory(destPath);
    if (mkdirResult == ERROR_CODE) {
        free(reversedName);
        closedir(dir);
        return ERROR_CODE;
    }

    int recursiveResult = workWithDirectory(srcPath, destPath);
    if (recursiveResult == ERROR_CODE) {
        free(reversedName);
        closedir(dir);
        return ERROR_CODE;
    }
    return 0;
}

int processEntryType(struct stat* sb, char* srcPath, char* destPath, char* reversedName, DIR* dir) {
    int processResult = 0;
    if (S_ISREG(sb->st_mode)) {
        processResult = processFile(srcPath, destPath, reversedName, dir);
    }

    if (S_ISDIR(sb->st_mode)) {
        processResult = processDirectory(srcPath, destPath, reversedName, dir);
    }

    if (processResult == ERROR_CODE) {
        closedir(dir);
        return ERROR_CODE;
    }
    return 0;
}

int processEntry(struct dirent *entry, char* srcDir, char* destDir, DIR *dir) {
    char srcPath[MAX_NAME_LEN];        
    int srcResult = buildPath(srcPath, MAX_NAME_LEN, srcDir, entry->d_name);
    if (srcResult == ERROR_CODE) { 
        closedir(dir);
        return reportError("build source path", srcPath);
    }        
        
    char* reversedName = createReversedName(entry->d_name);
    if (reversedName == NULL) {
        closedir(dir);
        return ERROR_CODE;
    }
         
    char destPath[MAX_NAME_LEN];
    int destResult = buildPath(destPath, MAX_NAME_LEN, destDir, reversedName);
    if (destResult == ERROR_CODE) { 
        closedir(dir);
        free(reversedName);
        return reportError("build destination path", destPath);
    }
            
    struct stat sb; 
    int statResult = lstat(srcPath, &sb);
    if (statResult == ERROR_CODE) {           
        free(reversedName); 
        closedir(dir);
        return reportError("get file info about", srcPath);       
    }          
     
    int processResult = processEntryType(&sb, srcPath, destPath, reversedName, dir);
    if (processResult == ERROR_CODE) {
        free(reversedName);
        return ERROR_CODE;
    }
    
    free(reversedName);
    return 0;
}

int workWithDirectory(char* srcPath, char* destPath) {
    DIR *dir = opendir(srcPath);  
    if (dir == NULL) {
        return reportError("open directory", srcPath);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {        
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        int processResult = processEntry(entry, srcPath, destPath, dir);
        if (processResult == ERROR_CODE) {
            return ERROR_CODE;
        }        
    }

    int closeDirResult = closedir(dir);
    if (closeDirResult == ERROR_CODE) {
        return reportError("close directiry", srcPath);        
    }    
    return 0;
}