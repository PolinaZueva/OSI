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
#define BUFFER_SIZE 4096

char* reverseString(char* str) {
    size_t length = strlen(str);
    char* reversedStr = (char*)malloc((length + 1) * sizeof(char));
    if (reversedStr == NULL) {
        perror("reverseString: Ошибка выделения памяти");
        return NULL;
    }
    for (size_t i = 0; i < length; i++) {
        reversedStr[i] = str[length - i - 1];
    }
    reversedStr[length] = '\0';
    return reversedStr;
}

void reverseBuffer(char* buffer, ssize_t size) {
    for (ssize_t i = 0; i < size / 2; i++) {
        char tempBuffer = buffer[i];
        buffer[i] = buffer[size - i - 1];
        buffer[size - i - 1] = tempBuffer;
    }
}

void copyAndReverseFileContent(char* srcPath, char* destPath) {
    int fdSrcPath = open(srcPath, O_RDONLY);
    if (fdSrcPath == -1) {
        perror("copyAndReverseFileContent: Ошибка открытия исходного файла");
        return;
    } 

    int fdDestPath = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, 0644); 
    if (fdDestPath == -1) {
        perror("CopyAndReverseFileContent: Ошибка открытия и создания файла");
        close(fdSrcPath);
        return;
    }

    off_t sizeFile = lseek(fdSrcPath, 0, SEEK_END);
    if (sizeFile == -1) {
        perror("CopyAndReverseFileContent: Ошибка в получение размера файла");
        close(fdSrcPath);
        close(fdDestPath);
        return;
    }

    char buffer[BUFFER_SIZE];
    while (sizeFile > 0) {
        ssize_t chunkBuffer = sizeFile > BUFFER_SIZE ? BUFFER_SIZE : sizeFile;
        sizeFile = sizeFile - chunkBuffer;
        lseek(fdSrcPath, sizeFile, SEEK_SET);
        if (read(fdSrcPath, buffer, chunkBuffer) != chunkBuffer) {
            perror("CopyAndReverseFileContent: Ошибка в чтении файла");
            close(fdSrcPath);
            close(fdDestPath);
            return;
        }

        reverseBuffer(buffer, chunkBuffer);

        if (write(fdDestPath, buffer, chunkBuffer) != chunkBuffer) {
            perror("CopyAndReverseFileContent: Ошибка записи в файл");
            close(fdSrcPath);
            close(fdDestPath);
            return;
        }
    }

    close(fdSrcPath);
    close(fdDestPath);
}

int buildPath(char* path, size maxNameLen, char* origDir, char* name) {
    char srcPath[MAX_NAME_LEN]
}

void workWithDirectory(char* origDir, char* finalDir) {
    DIR *dir = opendir(origDir);  
    if (dir == NULL) {
        perror("workWithDirectory: Ошибка открытия каталога");
        return;
    }

    struct dirent *content;
    while ((content = readdir(dir)) != NULL) {        
        if (strcmp(content->d_name, ".") == 0 || strcmp(content->d_name, "..") == 0) {
            continue;
        }

        char srcPath[MAX_NAME_LEN], destPath[MAX_NAME_LEN];
        int origLen = snprintf(srcPath, MAX_NAME_LEN, "%s/%s", origDir, content->d_name);
        if (origLen < 0 || origLen >= MAX_NAME_LEN) {
            fprintf(stderr, "workWithDirectory: Путь слишком длинный: %s/%s\n", origDir, content->d_name);
            continue;
        }     
        char* reversedNameFile = reverseString(content->d_name);
        if (reversedNameFile == NULL) continue;
        int finalLen = snprintf(destPath, MAX_NAME_LEN, "%s/%s", finalDir, reversedNameFile);
        if (finalLen < 0 || finalLen >= MAX_NAME_LEN) {
            fprintf(stderr, "workWithDirectory: Путь слишком длинный: %s/%s\n", finalDir, reversedNameFile);
            free(reversedNameFile);
            continue;
        }           
        struct stat sb; 
        if (lstat(srcPath, &sb) == -1) {
            perror("workWithDirectory: Ошибка получения информацияя о файле");
            free(reversedNameFile);
            continue;
        }              
        
        if (S_ISREG(sb.st_mode)) {
            copyAndReverseFileContent(srcPath, destPath);
        }
        if (S_ISDIR(sb.st_mode) && (mkdir(destPath, sb.st_mode & 0755) != -1 || errno == EEXIST)) {
            workWithDirectory(srcPath, destPath);
        }
        free(reversedNameFile);
    }
    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Используй %s PathNameDirectory\n", argv[0]);
        return -1;
    }    

    struct stat sb;    
    if (lstat(argv[1], &sb) == -1) {
        perror("main: Ошибка в получении информации о директории");
        return -1;
    } 
    if (!S_ISDIR(sb.st_mode)) {
        fprintf(stderr, "main: Путь не является каталогом\n");
        return -1;
    }

    char* reversedNameDir = reverseString(argv[1]);
    if (!reversedNameDir) {
        return -1;
    }

    if (mkdir(reversedNameDir, sb.st_mode & 0755) == -1 && errno != EEXIST) {  
        perror("main: Ошибка создания каталога\n");
        free(reversedNameDir);
        return -1;
    }

    workWithDirectory(argv[1], reversedNameDir);    
    free(reversedNameDir);
    return 0;
}