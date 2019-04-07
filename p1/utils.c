#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "lib/utils.h"
#include "lib/list.h"

// writes error message to STDERR
// caller is responsible for all cleanup!!
void do_error(void) {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// argv should be on the heap and should be pointers to the heap
// argv2 should also be a pointer to the heap
// if there's no redirect, redir_type = '\0', argc2 = 0, and argv2 = NULL
struct redirect* make_redirect(int argc, char** argv,
        char redir_type, int argc2, char** argv2) {
    struct redirect* redir = malloc(sizeof(struct redirect));
    redir->argc = argc;
    redir->argv = argv;
    redir->redir_type = redir_type;
    redir->argc2 = argc2;
    redir->argv2 = argv2;
    return redir;
}

void free_redirect(struct redirect* redir) {
    for (int i = 0; i < redir->argc; i++) {
        free((redir->argv)[i]);
    }
    free(redir->argv);
    for (int i = 0; i < redir->argc2; i++) {
        free((redir->argv2)[i]);
    }
    free(redir->argv2);
    free(redir);
}

void print_redirect(struct redirect* redir) {
    printf(" *  argc: %d\n", redir->argc);
    printf("    argv: [");
    for (int i = 0; i < redir->argc - 1; i++) {
        printf("'%s', ", redir->argv[i]);
    }
    printf("'%s']\n", redir->argv[redir->argc-1]);
    if (redir->redir_type != '\0') {
        printf("    rtyp: %c\n", redir->redir_type);
        printf("    argc2: %d\n", redir->argc2);
        printf("    argv2 : [");
        for (int i = 0; i < redir->argc2 - 1; i++) {
            printf("'%s', ", redir->argv2[i]);
        }
        printf("'%s']\n", redir->argv2[redir->argc2-1]);
    } else {
        printf("    no redirection\n");
    }
}

struct many_commands* make_many_commands(struct list* commands, char type) {
    struct many_commands* many_com = malloc(sizeof(struct many_commands));
    many_com->commands = commands;
    many_com->join_type = type;
    return many_com;
}

void free_command_list(struct list* commands) {
    int redir_len = commands->length;
    struct redirect* redirect;

    // empty out the list and free things
    for (int j = 0; j < redir_len; j++) {
        redirect = (struct redirect *)delete_element(commands, 0);
        free_redirect(redirect);
    }
    // free the list itself
    free_list(commands);
}

void free_many_commands(struct many_commands* many_com) {
    free_command_list(many_com->commands);
    free(many_com);
}

void print_many_commands(struct many_commands* many_com) {
    printf("join type: %c\n", many_com->join_type);
    printf("redirect structs:\n");

    struct redirect* redir;
    for (int i = 0; i < (many_com->commands)->length; i++) {
        redir = get_element(many_com->commands, i);
        print_redirect(redir);
    }
}
