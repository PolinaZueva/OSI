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