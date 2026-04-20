#include "main.h"
#include "utils.h"
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// constants
#define KEY "TODO"
#define BLACKLIST                                                              \
{".git", "node_modules", "target", "python3", "venv",                        \
    "nob",  ".cargo",       ".cache", ".html",   ".toml"}
#define LINE_BUF_SIZE 512
#define CONTENT_BUF_SIZE 8024

typedef enum {
    IS_FILE,
    IS_DIR,
    IS_ELSE,
} PathType;

typedef struct {
    int line;
    char *path;
    char *content;
} TodoItem;

typedef struct {
    size_t size;
    size_t capacity;
    TodoItem *items;
} TodoList;

PathType check_path_type(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        AL_error("Path does not exist: %s", path);
    }

    if (S_ISREG(path_stat.st_mode)) {
        // AL_log("Path is a file");
        return IS_FILE;
    } else if (S_ISDIR(path_stat.st_mode)) {
        // AL_log("Path is a directory");
        return IS_DIR;
    } else {
        // AL_log("Path is neither a regular file nor a directory");
        return IS_ELSE;
    }
}

void TodoItem_print(TodoItem *i) {
    printf("\x1b[1;1m%s:%d\n\x1b[0m", i->path, i->line);
    printf("%s", i->content);
    if (i->content[0] != '\0' && i->content[strlen(i->content) - 1] != '\n') {
        printf("\n");
    }
    printf("-----------------\n");
}

#define TODOLIST_INIT_SIZE 8

void TodoList_init(TodoList **arr_ptr) {
    TodoList *container = malloc(sizeof(TodoList));
    if (!container) {
        AL_error("Memory allocation failed\n");
    }

    container->size = 0;
    container->capacity = TODOLIST_INIT_SIZE;
    container->items = malloc(TODOLIST_INIT_SIZE * sizeof(TodoItem));
    if (!container->items) {
        free(container);
        AL_error("Memory allocation failed\n");
    }

    *arr_ptr = container;
}

void TodoList_insertItem(TodoList *tdl, TodoItem item) {
    if (tdl->size == tdl->capacity) {
        size_t new_capacity = tdl->capacity << 1;
        TodoItem *new_items = realloc(tdl->items, new_capacity * sizeof(TodoItem));
        if (!new_items) {
            AL_error("Out of memory\n");
        }
        tdl->items = new_items;
        tdl->capacity = new_capacity;
    }

    tdl->items[tdl->size++] = item;
}

void TodoList_print(TodoList *tdl) {
    for (size_t i = 0; i < tdl->size; i++) {
        TodoItem_print(&tdl->items[i]);
    }
}

void TodoList_free(TodoList *tdl) {
    if (!tdl) {
        return;
    }

    for (size_t i = 0; i < tdl->size; i++) {
        free(tdl->items[i].path);
        free(tdl->items[i].content);
    }

    free(tdl->items);
    free(tdl);
}

bool contains(const char *str, const char *word) {
    return strstr(str, word) != NULL;
}

bool is_empty_or_whitespace(const char *str) {
    if (str == NULL)
        return true;
    while (*str) {
        if (!isspace((unsigned char)*str)) {
            return false;
        }
        str++;
    }
    return true;
}

// If a line contains "TODO": collect that line and all following non-empty
// lines. Stop when an empty line is found.
void scan_file(TodoList *tdl, const char *path) {
    FILE *file = fopen(path, "r");
    char line[LINE_BUF_SIZE];

    if (!file) {
        AL_error("Unable to open file: %s", path);
    }

    int line_nr = 1;
    int todo_line = 0;
    bool collecting = false;
    char content[CONTENT_BUF_SIZE];

    content[0] = '\0';

    while (fgets(line, sizeof(line), file)) {
        if (collecting) {
            if (is_empty_or_whitespace(line)) {
                TodoItem item;
                item.line = todo_line;
                item.path = malloc(strlen(path) + 1);
                item.content = malloc(strlen(content) + 1);

                if (!item.path || !item.content) {
                    free(item.path);
                    free(item.content);
                    fclose(file);
                    AL_error("Memory allocation failed while building TODO item");
                }

                strcpy(item.path, path);
                strcpy(item.content, content);

                TodoList_insertItem(tdl, item);

                collecting = false;
                content[0] = '\0';
            } else {
                size_t content_len = strlen(content);
                size_t line_len = strlen(line);

                if (content_len + line_len + 1 < CONTENT_BUF_SIZE) {
                    strcat(content, line);
                } else {
                    fclose(file);
                    AL_error("TODO content too large in file: %s", path);
                }
            }
        }

        if (!collecting && contains(line, KEY)) {
            collecting = true;
            todo_line = line_nr;
            content[0] = '\0';

            if (strlen(line) + 1 < CONTENT_BUF_SIZE) {
                strcat(content, line);
            } else {
                fclose(file);
                AL_error("line too large in file: %s", path);
            }

            // AL_log("new todo initiated at %d", line_nr);
        }

        line_nr++;
    }

    // handle TODO that reaches EOF without a trailing blank line
    if (collecting && content[0] != '\0') {
        TodoItem item;
        item.line = todo_line;
        item.path = malloc(strlen(path) + 1);
        item.content = malloc(strlen(content) + 1);

        if (!item.path || !item.content) {
            free(item.path);
            free(item.content);
            fclose(file);
            AL_error("memory allocation failed while finalizing TODO item");
        }

        strcpy(item.path, path);
        strcpy(item.content, content);

        TodoList_insertItem(tdl, item);
    }

    fclose(file);
}

typedef struct {
    PathType type;
    char *dest;
} Path;

void walk_dir(TodoList *tdl, char *target_path) {
    struct dirent *file_info;
    DIR *dir = opendir(target_path);
    if (dir == NULL) {
        return;
    }
    while ((file_info = readdir(dir)) != 0) {
        if (strcmp(file_info->d_name, ".") == 0 ||
                strcmp(file_info->d_name, "..") == 0) {
            continue;
        }
        // ignore files/dirs with blacklisted terms
        char *blkl[] = BLACKLIST;
        bool skip = false;
        for (size_t i = 0; i < sizeof(blkl) / sizeof(blkl[0]); i++) {
            if (contains(file_info->d_name, blkl[i])) {
                // printf("Skipping file \"%s\" due to blacklist: %s\n",
                // file_info->d_name, blkl[i]);
                skip = true;
                break;
            }
        }
        if (skip) {
            continue;
        }

        char file_path[1000];
        snprintf(file_path, sizeof(file_path), "%s/%s", target_path,
                file_info->d_name);
        switch (check_path_type(file_path)) {
            case IS_FILE:
                scan_file(tdl, file_path);
            case IS_DIR:
                walk_dir(tdl, file_path);
            case IS_ELSE:
            default: {
                     }
        }
        // printf("%s\n", file_path);
    }
    closedir(dir);
}

void search(char *target_path) {
    // AL_log("the search is on inside: %s", target_path);

    PathType target_path_type = check_path_type(target_path);

    TodoList *tdl;
    TodoList_init(&tdl);

    switch (target_path_type) {
        case IS_DIR:
            walk_dir(tdl, target_path);
            break;

        case IS_FILE:
            scan_file(tdl, target_path);
            break;

        case IS_ELSE:
        default:
            TodoList_free(tdl);
            AL_error("Unexpected malfunction");
            break;
    }

    TodoList_print(tdl);
    TodoList_free(tdl);
}
