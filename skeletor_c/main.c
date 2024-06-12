#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


#define MAX_PATH_LEN 4096
#define COMMAND_BUFFER_SIZE 100000



void get_paths(char *base_path, char **dirs, int *dir_count, char **files, int *file_count);
void mkdirCommand(char **dirs, int dir_count, char *command);
void touchCommand(char **files, int file_count, char *command);
void copy_to_clipboard(const char *command);

int main() {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    char *dirs[1000];
    char *files[1000];
    int dir_count = 0;
    int file_count = 0;

    // alloc
    for (int i = 0; i < 1000; i++) {
        dirs[i] = malloc(MAX_PATH_LEN);
        files[i] = malloc(MAX_PATH_LEN);
        if (dirs[i] == NULL || files[i] == NULL) {
            perror("malloc");
            return 1;
        }
    }

    get_paths(".", dirs, &dir_count, files, &file_count);

    char mkdir_command[COMMAND_BUFFER_SIZE] = "mkdir -p ";
    mkdirCommand(dirs, dir_count, mkdir_command);

    char touch_command[COMMAND_BUFFER_SIZE] = "touch ";
    touchCommand(files, file_count, touch_command);

    char final_command[COMMAND_BUFFER_SIZE];
    snprintf(final_command, COMMAND_BUFFER_SIZE, "%s && %s", mkdir_command, touch_command);

    printf("Copied to clipboard\n");
    copy_to_clipboard(final_command);

    // 3 alloc
    for (int i = 0; i < 1000; i++) {
        free(dirs[i]);
        free(files[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Elapsed time: %.6f seconds\n", elapsed_time);

    return 0;
}

void get_paths(char *base_path, char **dirs, int *dir_count, char **files, int *file_count) {
    struct dirent *dp;
    DIR *dir = opendir(base_path);

    if (!dir) return;

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue;

        char path[MAX_PATH_LEN];
        snprintf(path, sizeof(path), "%s/%s", base_path, dp->d_name);

        struct stat path_stat;
        stat(path, &path_stat);

        if (S_ISDIR(path_stat.st_mode)) {
            if (*dir_count < 1000) {
                strncpy(dirs[*dir_count], path, MAX_PATH_LEN - 1);
                dirs[*dir_count][MAX_PATH_LEN - 1] = '\0'; // Ensure null-termination
                (*dir_count)++;
                get_paths(path, dirs, dir_count, files, file_count);
            } else {
                fprintf(stderr, "Directory limit reached\n");
                break;
            }
        } else if (S_ISREG(path_stat.st_mode)) {
            if (*file_count < 1000) {
                strncpy(files[*file_count], path, MAX_PATH_LEN - 1);
                files[*file_count][MAX_PATH_LEN - 1] = '\0'; // Ensure null-termination
                (*file_count)++;
            } else {
                fprintf(stderr, "File limit reached\n");
                break;
            }
        }
    }
    closedir(dir);
}

void mkdirCommand(char **dirs, int dir_count, char *command) {
    for (int i = 0; i < dir_count; i++) {
        strncat(command, dirs[i], COMMAND_BUFFER_SIZE - strlen(command) - 2);
        strncat(command, " ", COMMAND_BUFFER_SIZE - strlen(command) - 2);
    }
}

void touchCommand(char **files, int file_count, char *command) {
    for (int i = 0; i < file_count; i++) {
        strncat(command, files[i], COMMAND_BUFFER_SIZE - strlen(command) - 2);
        strncat(command, " ", COMMAND_BUFFER_SIZE - strlen(command) - 2);
    }
}

void copy_to_clipboard(const char *command) {
    char clipboard_command[COMMAND_BUFFER_SIZE];
    snprintf(clipboard_command, COMMAND_BUFFER_SIZE, "echo '%s' | pbcopy", command);
    system(clipboard_command);
}

