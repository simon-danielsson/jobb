#include "main.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

bool is_valid_path(const char *s) {
    struct stat path_stat;

    if (stat(s, &path_stat) != 0) {
        // path does not exist or is not accessible
        return false;
    }

    // path is:
    if (S_ISREG(path_stat.st_mode)) {
        // regular file
        return true;
    } else if (S_ISDIR(path_stat.st_mode)) {
        // directory
        return true;
    } else {
        // something else (symlink, socket etc.)
        return false;
    }
    return true;
}

void print_help() {
    printf("%s v%s\n", PRG_N, PRG_V);
    printf("%s\n%s\n", PRG_L, PRG_R);
    printf("--------------\n");
    printf("$ jobb help   : view help\n");
    printf("$ jobb <path> : specify a file or directory\n");
}
