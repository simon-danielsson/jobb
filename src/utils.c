#include "main.h"

char *remove_comment_prefixes_and_whitespace_at_beginning(char *input) {
    if (input == NULL) {
        return NULL;
    }

    char *p = input;

    while (*p && isspace((unsigned char)*p)) {
        p++;
    }

    if (strncmp(p, "//", 2) == 0) {
        p += 2;
    } else if (strncmp(p, "/**", 3) == 0) {
        p += 3;
    } else if (strncmp(p, "/*", 2) == 0) {
        p += 2;
    } else if (*p == '*') {
        p += 1;
    }

    while (*p && isspace((unsigned char)*p)) {
        p++;
    }

    memmove(input, p, strlen(p) + 1);
    return input;
}

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
    printf("$ jobb        : run from current directory\n");
    printf("$ jobb help   : view help\n");
    printf("$ jobb <path> : specify a target file or directory\n");
}
