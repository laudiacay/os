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

// todo: leaking file descriptors?
int* run_piped_redirect(struct redirect* redir, int* pid) {
    int save_stdin = dup(0);
    int save_stdout = dup(1);
    int in = 0;
    int out = 1;
    int stat;
    int pipefd[2];
    struct command* cmd;


    for (int i = 0; i < redir->commands->length - 1; i++) {
        pipe(pipefd); // todo: check for failure here
        out = pipefd[1];
        dup2(out, 1);
        close(out);
        cmd = get_element(redir->commands, i);

        if (!run_builtin(cmd->argc, cmd->argv)) {
            pid = malloc(sizeof(int));
            // todo: catch malloc fail
            if ((*pid = fork()) == 0) {
                execvp(cmd->argv[0], cmd->argv);
                do_error();
                free(pid);
                exit(0);
            }
            waitpid(*pid, &stat, 0);
            free(pid);
        }

        in = pipefd[0]; // set in for next process
        dup2(in, 0);
        close(in);
    }

    dup2(save_stdout, 1);
    close(save_stdout);

    cmd = get_element(redir->commands, redir->commands->length - 1);

    if (!run_builtin(cmd->argc, cmd->argv)) {
        pid = malloc(sizeof(int));
        // todo: catch malloc fail
        if ((*pid = fork()) == 0) {
            execvp(cmd->argv[0], cmd->argv);
            do_error();
            free(pid);
            exit(0);
        }
    }

    dup2(save_stdin, 0);
    close(save_stdin);
    return pid;
}

// todo: check on >, you totally fucking broke it lmao
int* run_redirect(struct redirect* redir, int* pid) {
    int outfile = 0;
    int stdout_save = 0;
    if (redir->redir_type == '|') {
        pid = run_piped_redirect(redir, pid);
        return pid;
    }

    struct command* cmd = get_element(redir->commands, 0);

    if (redir->redir_type == '>') {
        outfile = open(redir->out_file, O_CREAT|O_RDWR|O_TRUNC);
        if (outfile == -1) {
            do_error();
            exit(0);
        }
        stdout_save = dup(1);
        dup2(outfile, 1);
    }

    if (!run_builtin(cmd->argc, cmd->argv)) {
        pid = malloc(sizeof(int));
        // todo: catch malloc fail
        if ((*pid = fork()) == 0) {
            execvp(cmd->argv[0], cmd->argv);
            do_error();
            free(pid);
            exit(0);
        }
    } else pid = NULL;

    if (redir->redir_type == '>') {
        close(outfile);
        dup2(stdout_save, 1);
        close(stdout_save);
    }

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
