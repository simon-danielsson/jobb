#include "main.h"
#include "search.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    char target_path[100] = {0};

    if (argc == 1) {
        // AL_log("Executing program without added arguments...");
        if (target_path[0] == '\0') {
            target_path[0] = '.';
        }
        search(target_path);
        exit(0);
    }

    if (argc > 2) {
        AL_error("Too many arguments were passed!");
    }

    if (argc == 2) {
        for (int i = 1; i < argc; i++) {
            char arg[32] = {0};
            snprintf(arg, sizeof(arg), "%s", argv[i]);

            if (!strcmp(arg, "help")) {
                print_help();
                exit(0);
            }

            // assume arg is a target path if not "help"
            if (strcmp(arg, "help") && is_valid_path(arg)) {
                memcpy(target_path, arg, sizeof(arg));
                break;
            } else {
                AL_error("Path is not valid or accessible!");
                break;
            }
        }
    }

    search(target_path);

    return 0;
}
