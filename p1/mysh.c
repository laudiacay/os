#include <stdlib.h>
#include <stdio.h>
#include "lib/parsing.h"
#include "lib/utils.h"
#include "lib/run.h"

void one_iter(char* cmd_input_buf, struct many_commands* cmds, int batch_mode) {
    if (!batch_mode) printf("520sh> ");

    if (!(cmd_input_buf = read_until_newline())) continue;

    if (batch_mode) printf("%s\n", cmd_input_buf);

    // go ahead and parse it
    if ((cmds = parse_input_buffer(cmd_input_buf)) == NULL) {
        free(cmd_input_buf);
        continue;
    }

    // run it
    run_many_commands(cmds);

    // clean up
    free(cmd_input_buf);
    free_many_commands(cmds);
}

int main(int argc, char* argv[]) {
    // handle bad input
    if (argc > 2) {
        do_error();
        return 1;
    }

    // space to store command, plus null terminator
    char* cmd_input_buf;
    struct many_commands* cmds;

    if (argc == 2) {
        char* batch_filename = argv[1];
        FILE* batch_file = open(batch_filename);
        dup2(1, batch_file); // is this right um
        while (!feof(batch_file)) {
            one_iter(cmd_input_buf, cmds, 1);
        }
        dup2(batch_file, 1);
    } else {
        while (1) {
            one_iter(cmd_input_buf, cmds, 0);
        }
    }

    return 0;
}
