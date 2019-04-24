#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "lib/parsing.h"
#include "lib/utils.h"
#include "lib/run.h"

void one_iter(char* cmd_input_buf, struct many_commands* cmds, int batch_mode,
        FILE* fp) {
    if (!batch_mode) printf("520sh> ");

    cmd_input_buf = read_until_newline(batch_mode, fp);

    if (!cmd_input_buf) return;

    if (batch_mode) printf("%s\n", cmd_input_buf);

    // go ahead and parse it
    if (!(cmds = parse_input_buffer(cmd_input_buf))) {
        free(cmd_input_buf);
        return;
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
    char* cmd_input_buf = NULL;
    struct many_commands* cmds;

    if (argc == 2) {
        char* batch_filename = argv[1];
        char mode[2] = "r";
        FILE* fp = fopen(batch_filename, mode);
        if (!fp) {
            do_error();
            return 1;
        }
        while (!feof(fp)) {
            one_iter(cmd_input_buf, cmds, 1, fp);
        }
        fclose(fp);
    } else {
        while (1) {
            one_iter(cmd_input_buf, cmds, 0, NULL);
        }
    }

    return 0;
}
