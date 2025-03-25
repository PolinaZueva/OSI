#include <stdio.h>
#include <sys/stat.h>
#include "dir_funcs.h"
#include "helpers.h"

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