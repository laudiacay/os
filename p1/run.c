#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "lib/utils.h"
#include "lib/list.h"
#include "lib/builtins.h"

// returns 0 if no builtin was matched and run
// returns 1 if a builtin was matched and run
int run_builtin(int argc, char** argv) {
    if (!strcmp(argv[0], "quit")) {
        if (argc != 1) do_error();
        else quit();
    } else if (!strcmp(argv[0], "pwd")) {
        if (argc != 1) do_error();
        else pwd();
    } else if (!strcmp(argv[0], "cd")) {
        if (argc > 2) do_error();
        else {
            if (argc == 2) cd(argv[1]);
            else cd(NULL);
        }
    } else if (!strcmp(argv[0], "echo")) {
        if (argc != 2) do_error();
        else echo(argv[1]);
    } else return 0;

    return 1;
}

int* run_redirect(struct redirect* redir, int* pid) {
    // todo: implement pipe
    int outfile = 0;
    if (redir->redir_type == '|') return NULL;

    struct command* cmd = get_element(redir->commands, 0);
    if (!run_builtin(cmd->argc, cmd->argv)) {
        pid = malloc(sizeof(int));
        if ((*pid = fork()) == 0) {
            if (redir->redir_type == '>') {
                // todo: what the hell are these file permissions
                outfile = open(redir->out_file, O_CREAT|O_WRONLY|O_TRUNC);
                if (outfile == -1) {
                    do_error();
                    free(pid);
                    exit(0);
                }
                dup2(outfile, 1);
            }
            execvp(cmd->argv[0], cmd->argv);
            do_error();
            free(pid);
            exit(0);
        }
    } else pid = NULL;

    return pid;
}

void run_parallel_commands(struct list* redirects) {
    struct list* pid_list = init_list();
    struct redirect* cur_redirect;
    int* pid = NULL;

    for (int i = 0; i < redirects->length; i++) {
        cur_redirect = (struct redirect*)get_element(redirects, i);
        if (cur_redirect == NULL) {
            do_error();
            continue;
        }

        pid = run_redirect(cur_redirect, pid);
        if (pid) add_elem(pid_list, pid_list->length, pid);
    }

    int stat;
    for (int i = 0; i < pid_list->length; i++) {
        pid = (int*)get_element(pid_list, i);
        waitpid(*pid, &stat, 0);
        free(pid);
    }

    free_list(pid_list);
}

void run_serial_commands(struct list* redirects) {
    int *pid = NULL;
    int stat;
    struct redirect* cur_redirect;

    for (int i = 0; i < redirects->length; i++) {
        cur_redirect = (struct redirect*)get_element(redirects, i);
        if (cur_redirect == NULL) {
            do_error();
            continue;
        }

        pid = run_redirect(cur_redirect, pid);

        if (pid) {
            waitpid(*pid, &stat, 0);
            free(pid);
        }
    }
}

// run one line represented as a many_commands struct
void run_many_commands(struct many_commands* many_coms) {
    if (many_coms->join_type == '+') {
        run_parallel_commands(many_coms->redirects);
    } else {
        run_serial_commands(many_coms->redirects);
    }
}
