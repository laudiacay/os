#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "lib/utils.h"

void quit() {
    exit(0);
}

void pwd() {
    char* cwd = malloc(MAX_PATH_LENGTH);
    getcwd(cwd, MAX_PATH_LENGTH);
    printf("%s\n", cwd);
    free(cwd);
}

void cd(char* dest) {
    if (dest == NULL) {
        dest = getenv("HOME");
    }

    if (chdir(dest)) do_error();
}

void echo(char* string) {
    printf("%s\n", string);
}
