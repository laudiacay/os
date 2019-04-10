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

struct command* make_command(int argc, char** argv) {
    struct command* cmd = malloc(sizeof(struct command));
    cmd->argc = argc;
    cmd->argv = argv;
    return cmd;
}

void free_command(struct command* cmd) {
    if (!cmd) return;

    for (int i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
    free(cmd->argv);
    free(cmd);
}

void print_command(struct command* cmd) {
    printf("        argc: %d\n", cmd->argc);
    printf("        argv: [");
    for (int i = 0; i < cmd->argc - 1; i++) {
        printf("'%s', ", cmd->argv[i]);
    }
    printf("'%s']\n", cmd->argv[cmd->argc-1]);
}

// if there's no redirect, redir_type = '\0'
struct redirect* make_redirect(struct list* commands, char redir_type,
        char* out_file) {
    struct redirect* redir = malloc(sizeof(struct redirect));
    redir->commands = commands;
    redir->redir_type = redir_type;
    redir->out_file = out_file;
    return redir;
}

void free_redirect(struct redirect* redir) {
    if (!redir) return;
    for (int i = 0; i < (redir->commands)->length; i++) {
        free_command((struct command*)get_element(redir->commands, i));
    }
    free_list(redir->commands);
    free(redir->out_file);
    free(redir);
}

void print_redirect(struct redirect* redir) {
    printf(" *  commands:\n");
    for (int i = 0; i < redir->commands->length; i++) {
        print_command((struct command*)get_element(redir->commands, i));
    }
    if (redir->redir_type != '\0') {
        printf("    rtyp: %c\n", redir->redir_type);
        if (redir->redir_type == '>') printf("    out_file: %s\n", redir->out_file);
    } else {
        printf("    no redirection\n");
    }
}

struct many_commands* make_many_commands(struct list* redirects, char type) {
    struct many_commands* many_com = malloc(sizeof(struct many_commands));
    many_com->redirects = redirects;
    many_com->join_type = type;
    return many_com;
}

void free_many_commands(struct many_commands* many_com) {
    for (int i = 0; i < (many_com->redirects)->length; i++) {
        free_redirect((struct redirect*)get_element(many_com->redirects, i));
    }
    free_list(many_com->redirects);
    free(many_com);
}

void print_many_commands(struct many_commands* many_com) {
    printf("join type: %c\n", many_com->join_type);
    printf("redirect structs:\n");

    struct redirect* redir;
    for (int i = 0; i < (many_com->redirects)->length; i++) {
        redir = get_element(many_com->redirects, i);
        print_redirect(redir);
    }
}
