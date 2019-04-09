#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
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

void run_parallel_commands(struct list* cmd_list) {
    int pid_arr[cmd_list->length];
    struct redirect* cur_com;

    for (int i = 0; i < cmd_list->length; i++) {
        cur_com = (struct redirect*)get_element(cmd_list, i);
        // todo: add pipe/redirect support here
        if (!run_builtin(cur_com->argc, cur_com->argv)) {
            if ((pid_arr[i] = fork()) == 0) {
                exit(0);
                execvp(cur_com->argv[0], cur_com->argv);
            }
        }
    }

    int stat;
    for (int i = 0; i < cmd_list->length; i++) {
        waitpid(pid_arr[i], &stat, 0);
    }
}

void run_serial_commands(struct list* cmd_list) {
    int pid, stat;
    struct redirect* cur_com;

    for (int i = 0; i < cmd_list->length; i++) {
        cur_com = (struct redirect*)get_element(cmd_list, i);
        // todo: add pipe/redirect support here
        if (!run_builtin(cur_com->argc, cur_com->argv)) {
            if ((pid = fork()) == 0) {
                if (run_builtin(cur_com->argc, cur_com->argv)) exit(0);
                execvp(cur_com->argv[0], cur_com->argv);
                do_error();
            } else waitpid(pid, &stat, 0);
        }
    }
}

// run one line represented as a many_commands struct
void run_many_commands(struct many_commands* many_coms) {
    if (many_coms->join_type == '+') {
        run_parallel_commands(many_coms->commands);
    } else {
        run_serial_commands(many_coms->commands);
    }
}
