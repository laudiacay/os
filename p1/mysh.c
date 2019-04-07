#include <stdlib.h>
#include <stdio.h>
#include <lib/parsing.h>

MAX_INPUT_LENGTH = 256

// print out usage error, return error code
int usage() {
    printf("Usage: ./mysh\n");
    return 1;
}

int main(int argc, char *argv[]) {
    // handle bad input
    if (argc != 1)
        return usage();
    
    // space to store command, plus null terminator
    char* cmd_input_buf = malloc(MAX_INPUT_LENGTH+1);
    
    // store if we should quit
    int quit = 0;

    while (1) {
        if (read_until_newline(cmd_input_buf)) {
            // TODO: error, input line was too long
        }
        
        // handle whatever we read off STDIN, return if we should quit
        quit = do_handle_args(cmd_input_buf);
        
        // clean up and quit the loop if we're done
        if (quit) {
            free(cmd_input_buf);
            break;
        }
    }
    
    // return a status code
    return quit;
}
