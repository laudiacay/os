#include <stdlib.h>
#include <stdio.h>
#include "lib/parsing.h"
#include "lib/utils.h"
#include "lib/run.h"

int main() {
    // handle bad input
    /*if (argc > 2) {
        do_error();
        return 1;
    }*/

    // space to store command, plus null terminator
    char* cmd_input_buf;

    // store if we should quit
    int status = 0;

    while (1) {
        // print the prompt
        printf("520sh> ");

        // read in the input, if it failed, just go back to printing prompt
        if (!(cmd_input_buf = read_until_newline())) continue;

        struct many_commands* cmds;
        if ((cmds = parse_input_buffer(cmd_input_buf)) == NULL) {
            free(cmd_input_buf);
            continue;
        }

        run_many_commands(cmds);

        // todo: when should we break the loop?
        // todo: batch mode
        // todo: check all malloc failures and fork failures
        free(cmd_input_buf);
        free_many_commands(cmds);
    }

    // todo: return a status code?
    return status;
}
