#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "lib/utils.h"
#include "lib/list.h"

// clear stdin, for when the input is too long (munch until you hit the end lol)
void clear_stdin(int batch_mode, FILE* fp) {
    char c;
    char* print_command_in_batch_mode;
    while (1) {
        if (!batch_mode) c = getchar();
        else {
            c = fgetc(fp);
            print_command_in_batch_mode = &c;
            if (!write(STDOUT_FILENO, print_command_in_batch_mode, 1)) return;
        }
        if (c == EOF || c == '\n' || c == '\r') break;
    }
}

// read STDIN into buffer, of size MAX_INPUT_LENGTH + 1
// return NULL for error, buffer if everything went okay
char* read_until_newline(int batch_mode, FILE* fp) {

    // we want to load the input into a buffer
    char* cmd_input_buf = malloc(MAX_INPUT_LENGTH + 1);

    // malloc failed, error out and return error code
    if (!cmd_input_buf) {
        do_error();
        return NULL;
    }

    char c; // keep track of the current char
    int i = 0; // keep track of the position in the buffer
    while (1) {
        if (!batch_mode) c = getchar();
        else c = fgetc(fp);

        if (c == EOF) {
            free(cmd_input_buf);
            return NULL;
        }

        // are we done reading? if so, null terminate and return.
        if (c == '\n' || c == '\r') {
            cmd_input_buf[i] = '\0';
            return cmd_input_buf;
        }

        // put c in the buffer
        cmd_input_buf[i] = c;

        // did we walk off the end of the buffer?
        // if so, clear stdin, free buf, error out, return NULL.
        // if batch_mode, write it out to stdout. this is hack. ugh.
        if (i >= MAX_INPUT_LENGTH) {
            if (!write(STDOUT_FILENO, cmd_input_buf, i+1)) return NULL;
            clear_stdin(batch_mode, fp);
            free(cmd_input_buf);
            do_error();
            return NULL;
        }

        // otherwise, all is well, increment i
        i++;
    }
}

// check whether we're doing ; or + or neither
// returns '\0' if neither
// returns '\1' if error (spotted both tokens)
// returns ; or + corresponding to which one we're using
char which_join_type(char* in_buffer) {
    char join_token = '\0';
    int i = 0;
    while (in_buffer[i] != '\0') {
        if (in_buffer[i] == '+' || in_buffer[i] == ';') { // found + or ;
            if (join_token == '\0') join_token = in_buffer[i];
            else if (join_token != in_buffer[i]) {  // found both tokens, err
                join_token = '\1';
                break;
            }
        } i++;
    }
    return join_token;
}

// check whether we're doing | or > or neither
// returns '\0' if neither
// returns '\1' if error (spotted both tokens or multiple >)
// returns | or > corresponding to which one we're using
char which_redir_type(char* in_buffer) {
    char redir_token = '\0';
    int i = 0;
    while (in_buffer[i] != '\0') {
        if (in_buffer[i] == '|' || in_buffer[i] == '>') { // found + or ;
            if (redir_token == '\0') redir_token = in_buffer[i];
            // duplicate > or mismatched tokens
            else if (redir_token != in_buffer[i] || redir_token == '>') {
                redir_token = '\1';
                break;
            }
        } i++;
    }
    return redir_token;
}

// tokenize by +; and put the tokens in a list
struct list* joined_command_to_list(char *in_buffer) {
    char delims[3] = ";+";
    char* saveptr;
    char* token;

    struct list* list = init_list();

    token = strtok_r(in_buffer, delims, &saveptr);
    while (token != NULL) {
        add_elem(list, list->length, token);
        token = strtok_r(NULL, delims, &saveptr);
    }
    return list;
}

// get a command struct from a list of arguments
struct command* command_from_string(char* in_buffer) {
    struct list* argv_list = init_list();
    char delims[3] = "\t ";
    char* saveptr;
    char* token;

    token = strtok_r(in_buffer, delims, &saveptr);
    if (token == NULL) return NULL; // empty cmd
    while (token != NULL) {
        add_elem(argv_list, argv_list->length, token);
        token = strtok_r(NULL, delims, &saveptr);
    }

    char** argv = malloc((argv_list->length + 1)*sizeof(char*));
    if (!argv) {
        // todo: free argv_list
        do_error();
        return NULL;
    }
    char* argv_i;
    for (int i = 0; i < argv_list->length; i++) {
        argv_i = (char*)get_element(argv_list, i);
        argv[i] = malloc(strlen(argv_i) + 1);
        // todo: catch this malloc failure
        strcpy(argv[i], argv_i);
    }
    // null terminate argv
    argv[argv_list->length] = NULL;

    int argc = argv_list->length;
    free_list(argv_list);

    return make_command(argc, argv);
}

char* validate_and_clean_redir_filename(char* raw_filename) {
    if (!raw_filename) return NULL;

    char delims[3] = "\t ";
    char* saveptr;

    char* filename = strtok_r(raw_filename, delims, &saveptr);
    char* extra = strtok_r(NULL, delims, &saveptr);
    if (extra || !filename) return NULL;
    return filename;
}

struct redirect* build_arrow_redirect(char* input_string) {
    // breaks input string up into list of commands
    struct list* commands = init_list();
    char delims[2] = ">";
    char* saveptr;

    // get input file and put it into commands list
    char* before_redir = strtok_r(input_string, delims, &saveptr);
    struct command* com = command_from_string(before_redir);
    if (!com) {
        do_error();
        free_list(commands);
        return NULL;
    }
    add_elem(commands, commands->length, com);

    // grab output file and put it on the heap
    char* _after_redir = strtok_r(NULL, delims, &saveptr);
    // handle something like "ls >" or "ls > out1 out2 out3"
    _after_redir = validate_and_clean_redir_filename(_after_redir);

    if (!_after_redir) {
        for (int i = 0; i < commands->length; i++) {
            free_command(delete_element(commands, 0));
        }
        free_list(commands);
        return NULL;
    }
    // stick it on the heap for use later :)
    char* after_redir = malloc(strlen(_after_redir) + 1);
    // todo: catch this malloc failure

    strcpy(after_redir, _after_redir);

    return make_redirect(commands, '>', after_redir);
}

struct redirect* build_pipe_redirect(char* input_string) {
    // breaks input string up into list of commands
    struct list* commands = init_list();
    char delims[2] = "|";
    char* saveptr;
    char* token;
    struct command* com;

    token = strtok_r(input_string, delims, &saveptr);
    while (token != NULL) {
        com = command_from_string(token);
        if (!com) {
            // todo: what other cleanup?
            do_error();
            return NULL;
        }
        add_elem(commands, commands->length, com);
        token = strtok_r(NULL, delims, &saveptr);
    }

    return make_redirect(commands, '|', NULL);
}

// creates a redirect struct out of a string
// checks for bad formatting
// todo: fix all malloc, file, and fork failures
struct redirect* build_redirect(char* input_string) {
    char redir_type = which_redir_type(input_string);
    struct redirect* ret;
    if (redir_type == '\1') ret = NULL;
    else if (redir_type == '>') ret = build_arrow_redirect(input_string);
    else if (redir_type == '|') ret = build_pipe_redirect(input_string);
    else {
        struct list* commands = init_list();
        add_elem(commands, 0, command_from_string(input_string));
        // todo: catch if command_from_string failed;
        ret = make_redirect(commands, '\0', NULL);
    }
    return ret;
}

// build redirect list
struct list* build_redirect_list(struct list* joined_strings) {
    struct list* redir_list = init_list();
    struct redirect* redirect;

    for (int i = 0; i < joined_strings->length; i++) {
        redirect = build_redirect((char*)get_element(joined_strings, i));
        add_elem(redir_list, redir_list->length, redirect);
    }

    return redir_list;
}

// take input buffer, output list of strings parsed into tokens
struct many_commands* parse_input_buffer(char* in_buffer) {
    // calculate what kind of redirect we're doing
    char join_type = which_join_type(in_buffer);

    if (join_type == '\1') {  // error parsing joins
        do_error();
        return NULL;
    }

    struct list* joined_strings = joined_command_to_list(in_buffer);

    struct list* redirect_list = build_redirect_list(joined_strings);

    free_list(joined_strings);

    if (redirect_list == NULL) return NULL;

    struct many_commands* many_coms = malloc(sizeof(struct many_commands));
    if (!many_coms) {
        // todo: is this a memory leak?
        return NULL;
    }
    many_coms->redirects = redirect_list;
    many_coms->join_type = join_type;

    //print_many_commands(many_coms);

    return many_coms;
}
