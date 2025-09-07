#include "file_funcs.h"

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

int readFile(int fdSrc, char* buffer, ssize_t chunkBuffer, char* srcPath) {
    ssize_t countRead = 0;  //общее количество прочитанных байтов
    ssize_t bytesRead;
    while (countRead < chunkBuffer) {
        bytesRead = read(fdSrc, buffer + countRead, chunkBuffer - countRead);
        if (bytesRead == ERROR_CODE) {
            return reportError("read in source file", srcPath); 
        }
        if (bytesRead == 0) {  //конец файла
            break; 
        }
        countRead = countRead + bytesRead;
    }
    return countRead;
}

int writeFile(int fdDest, char* buffer, ssize_t chunkBuffer, char* destPath) {
    ssize_t countWrite = 0;
    ssize_t bytesWrite;
    while (countWrite < chunkBuffer) {
        bytesWrite = write(fdDest, buffer + countWrite, chunkBuffer - countWrite);
        if (bytesWrite == ERROR_CODE) {
            return reportError("write in destination file", destPath); 
        }
        countWrite = countWrite + bytesWrite;
    }
    return countWrite;
}

int copyAndReverseFileContent(int fdSrc, char* srcPath, int fdDest, char* destPath) {
    char buffer[BUFFER_SIZE];  //массив байтов
    off_t sizeFile = lseek(fdSrc, 0, SEEK_END);
    if (sizeFile == ERROR_CODE) {
        return reportError("get file size", srcPath);  
    }

    while (sizeFile > 0) {
        ssize_t chunkBuffer = sizeFile > BUFFER_SIZE ? BUFFER_SIZE : sizeFile;
        off_t displs = sizeFile - chunkBuffer;  //смещение от начала файла
        off_t seekResult = lseek(fdSrc, displs, SEEK_SET);
        if (seekResult == ERROR_CODE) {
            return reportError("seek in source file", srcPath);                
        }

        ssize_t readResult = readFile(fdSrc, buffer, chunkBuffer, srcPath);
        if (readResult == ERROR_CODE) {
            return ERROR_CODE; 
        } 

        reverseBuffer(buffer, chunkBuffer);

        ssize_t writeResult = writeFile(fdDest, buffer, chunkBuffer, destPath);
        if (writeResult == ERROR_CODE) {
            return ERROR_CODE;                
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

int checkReversedFile(char* srcPath, char* destPath) {
    int reversedResult = reverseFile(srcPath, destPath);
    if (reversedResult == ERROR_CODE) {
        return ERROR_CODE;
    }
    return 0;
}