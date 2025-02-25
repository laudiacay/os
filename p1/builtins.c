#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "lib/utils.h"

void quit() {
    exit(0);
}

void pwd() {
    char* newline = "\n";
    char* cwd = malloc(MAX_PATH_LENGTH);
    if (!getcwd(cwd, MAX_PATH_LENGTH)) {
        do_error();
        return;
    }
    if (!write(STDOUT_FILENO, cwd, strlen(cwd))) return;
    if (!write(STDOUT_FILENO, newline, strlen(newline))) return;
    free(cwd);
}

void cd(char* dest) {
    if (dest == NULL) {
        dest = getenv("HOME");
    }
    if (chdir(dest)) do_error();
}

void echo(char* string) {
    char* newline = "\n";
    if (!write(STDOUT_FILENO, string, strlen(string))) return;
    if (!write(STDOUT_FILENO, newline, strlen(newline))) return;
}
