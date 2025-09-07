#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define MAX_NAME_LEN 256
#define BUFFER_SIZE 2048
#define ERROR_MSG_SIZE 512
#define ERROR_CODE -1
#define FILE_ACCESS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)  //rw-rw-rw-
#define DIRECTORY_ACCESS (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)  //rwxr-xr-x

int workWithDirectory(char* srcPath, char* destPath);

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

int openFiles(int* fdSrc, char* srcPath, int* fdDest, char* destPath) {
    *fdSrc = open(srcPath, O_RDONLY);
    if (*fdSrc == ERROR_CODE) {
        return reportError("open source file", srcPath);        
    } 

    *fdDest = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, FILE_ACCESS);
    if (*fdDest == ERROR_CODE) {
        close(*fdSrc);
        return reportError("open and create destination file", destPath);    
        
    }    
    return 0;
}

int closeFiles(int fdSrc, char* srcPath, int fdDest, char* destPath) {
    int closeSrcResult = close(fdSrc);   
    if (closeSrcResult == ERROR_CODE) {
        close(fdDest);
        return reportError("close source file", srcPath);    
        
    } 

    int closeDestResult = close(fdDest);
    if (closeDestResult == ERROR_CODE) {
        return reportError("close destination file", destPath);    
    } 
    return 0;
}

int copyAndReverseFileContent(int fdSrc, char* srcPath, int fdDest, char* destPath) {
    char buffer[BUFFER_SIZE];
    off_t sizeFile = lseek(fdSrc, 0, SEEK_END);
    if (sizeFile == ERROR_CODE) {
        return reportError("get file size", srcPath);  
    }

    while (sizeFile > 0) {
        ssize_t chunkBuffer = sizeFile > BUFFER_SIZE ? BUFFER_SIZE : sizeFile;
        off_t displs = sizeFile - chunkBuffer;  //смещение от конца файла
        off_t seekResult = lseek(fdSrc, displs, SEEK_SET);
        if (seekResult == ERROR_CODE) {
            return reportError("seek in source file", srcPath);                
        }

        ssize_t readResult = read(fdSrc, buffer, chunkBuffer);
        if (readResult != chunkBuffer) {
            return reportError("read in source file", srcPath); 
        }

        reverseBuffer(buffer, chunkBuffer);

        ssize_t writeResult = write(fdDest, buffer, chunkBuffer);
        if (writeResult != chunkBuffer) {
            return reportError("write in destination file", destPath);                
        }
        sizeFile = sizeFile - chunkBuffer;
    }
    return 0;
}

int reverseFile(char* srcPath, char* destPath) {
    int fdSrc, fdDest;
    int openResult = openFiles(&fdSrc, srcPath, &fdDest, destPath);
    if (openResult == ERROR_CODE) {
        return ERROR_CODE;
    }

    int reverseResult = copyAndReverseFileContent(fdSrc, srcPath, fdDest, destPath);
    if (reverseResult == ERROR_CODE) {
        closeFiles(fdSrc, srcPath, fdDest, destPath);
        return ERROR_CODE;
    }
    
    int closeResult = closeFiles(fdSrc, srcPath, fdDest, destPath);
    if (closeResult == ERROR_CODE) {
        return ERROR_CODE;      
    } 
    return 0;
}

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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Use %s PathDirectory\n", argv[0]);
        return EXIT_FAILURE;
    }    

    struct stat pathInfo;    
    int statResult = stat(argv[1], &pathInfo);
    if (statResult == ERROR_CODE) {
        return reportError("get directiry info", argv[1]);
    } 

    if (S_ISDIR(pathInfo.st_mode) == 0) {
        fprintf(stderr, "Path is not a directory: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    char* reversedNameDir = createReversedName(argv[1]);
    if (reversedNameDir == NULL) { 
        return EXIT_FAILURE;
    }

    int mkdirResult = createDirectory(reversedNameDir);
    if (mkdirResult == ERROR_CODE) {
        free(reversedNameDir);
        return EXIT_FAILURE;
    }

    int workResult =  workWithDirectory(argv[1], reversedNameDir);
    free(reversedNameDir);
    if (workResult == ERROR_CODE) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}