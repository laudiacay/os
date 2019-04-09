#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib/utils.h"
#include "lib/list.h"

// clear stdin, for when the input is too long (munch until you hit the end lol)
void clear_stdin(void) {
    char c;
    while (1) {
        c = getchar();
        if (c == EOF || c == '\n' || c == '\r') {
            break;
        }
    }
}


// read STDIN into buffer, of size MAX_INPUT_LENGTH + 1
// return NULL for error, buffer if everything went okay
char *read_until_newline(void) {

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
        c = getchar();

        // are we done reading? if so, null terminate and return.
        if (c == EOF || c == '\n' || c == '\r') {
            cmd_input_buf[i] = '\0';
            return cmd_input_buf;
        }

        // did we walk off the end of the buffer?
        // if so, clear stdin, free buf, error out, return NULL.
        if (i >= MAX_INPUT_LENGTH) {
            clear_stdin();
            free(cmd_input_buf);
            do_error();
            return NULL;
        }

        // otherwise, all is well, put c into the buffer and increment i
        cmd_input_buf[i] = c;
        i++;
    }
}

// check whether we're doing ; or + or neither
// returns '\0' if neither
// returns '\1' if error (spotted both tokens)
// returns ; or + corresponding to which one we're using
char which_join_type(char* in_buffer) {
    // start by checking which one it is, ; or +
    char join_token = '\0';
    int i = 0;
    while (in_buffer[i] != '\0') {
        if (in_buffer[i] == '+' || in_buffer[i] == ';') { // found + or ;
            if (join_token == '\0') {  // first one we've seen
                join_token = in_buffer[i];
            } else if (join_token != in_buffer[i]) {  // found both tokens, err
                join_token = '\1';
                break;
            }
        }
        i++;
    }
    return join_token;
}

// tokenize by +; and put the tokens in a list
struct list* load_joined_strings_into_list(char *in_buffer) {
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

// calculate redirect type from input string
char which_redir_type(char* input_string) {
    for (unsigned int i = 0; i < strlen(input_string); i++) {
        if (input_string[i] == '>' || input_string[i] == '|') {
            return input_string[i];
        }
    }
    return '\0';
}

// tokenize the stuff before the >| into a list of args
struct list* tokenize_into_argv_list(char* before_redirect) {
    char delims[3] = "\t ";
    char* saveptr;
    char* token;

    struct list* arg_list = init_list();

    // read the args into a list
    token = strtok_r(before_redirect, delims, &saveptr);
    while (token != NULL) {
        add_elem(arg_list, arg_list->length, token);
        token = strtok_r(NULL, delims, &saveptr);
    }

    return arg_list;
}

// get a valid argv array from a list of arguments
char** argv_from_list(struct list* argv_list) {
    char** argv = malloc((argv_list->length + 1)*sizeof(char*));
    char* argv_i;
    for (int i = 0; i < argv_list->length; i++) {
        argv_i = (char*)get_element(argv_list, i);
        argv[i] = malloc(strlen(argv_i) + 1);
        strcpy(argv[i], argv_i);
    }

    // null terminate argv
    argv[argv_list->length] = NULL;
    return argv;
}

// todo: fix me for |
// this tokenizes the stuff after the |> into something usable
char** tokenize_file_redir_out(char* after_redirect) {
    char delims[3] = "\t ";
    char* saveptr;

    char* out = strtok_r(after_redirect, delims, &saveptr);
    if (strtok_r(NULL, delims, &saveptr) != NULL) return NULL;

    char** ret = NULL;
    if (out != NULL) {
        ret = malloc(sizeof(char*));
        ret[0] = malloc(strlen(out) + 1);
        strcpy(ret[0], out);
    }
    return ret;
}

// creates a redirect struct out of a string
// checks for bad formatting
// todo: handle multiple pipes
struct redirect* build_redirect(char* input_string) {
    char delims[3] = ">|";
    char* saveptr;
    char* before_redirect;
    char* after_redirect;
    char* third_part;
    char redir_type = which_redir_type(input_string);

    // make sure there are no more than 2 tokenized things from input_string
    before_redirect = strtok_r(input_string, delims, &saveptr);
    after_redirect = strtok_r(NULL, delims, &saveptr);

    if (after_redirect != NULL) {
        third_part = strtok_r(NULL, delims, &saveptr);
        if (third_part != NULL) {
            do_error();
            return NULL;
        }
    }

    // tokenize before_redirect and after_redirect by whitespace
    struct list* arg_list = tokenize_into_argv_list(before_redirect);
    int argc = arg_list->length;
    char** argv = argv_from_list(arg_list);

    int len = arg_list->length;
    char* current_str;
    for (int i = 0; i < len - 1; i++) {
        current_str = get_element(arg_list, i);
        printf("this element: %s\n", current_str);
        free(current_str);
    }
    free_list(arg_list);

    // handle after_redirect tokenization
    int argc2;
    char** argv2;
    if (redir_type == '|') {
        struct list* arg2_list = tokenize_into_argv_list(after_redirect);
        argc2 = arg2_list->length;
        argv2 = argv_from_list(arg2_list);
        len = arg2_list->length;
        for (int i = 0; i < len - 1; i++) {
            free(get_element(arg2_list, i));
        }
        free_list(arg2_list);
    } else if (redir_type == '>') {
        argc2 = 1;
        argv2 = tokenize_file_redir_out(after_redirect);
    } else {
        argc2 = 0;
        argv2 = NULL;
    }

    if (argv2 == NULL && redir_type != '\0') {
        free_list(arg_list);
        free(argv); // todo: are there still a bunch of strings on the heap?
        do_error();
        return NULL;
    }

    // create redirect struct
    struct redirect* redir = malloc(sizeof(struct redirect));
    redir->argc = argc;
    redir->argv = argv;
    redir->redir_type = redir_type;
    redir->argc2 = argc2;
    redir->argv2 = argv2;

    return redir;
}

// build command list
struct list* build_command_list(struct list* joined_strings) {
    struct list* redir_list = init_list();
    char* cmd_str;
    struct redirect* redirect;

    for (int i = 0; i < joined_strings->length; i++) {
        cmd_str = (char*)get_element(joined_strings, i);
        redirect = build_redirect(cmd_str);

        // failed, time to free things and leave
        if (redirect == NULL) {
            free_command_list(redir_list);
            return NULL;
        }

        add_elem(redir_list, redir_list->length, redirect);
    }

    return redir_list;
}

// take input buffer, output list of strings parsed into tokens
// todo: figure out the incompatible pointer error....
struct many_commands* parse_input_buffer(char* in_buffer) {
    // calculate what kind of redirect we're doing
    char join_type = which_join_type(in_buffer);

    if (join_type == '\1') {  // error parsing redirect
        do_error();
        return NULL;
    }

    struct list* joined_strings = load_joined_strings_into_list(in_buffer);

    struct list* command_list = build_command_list(joined_strings);
    free_list(joined_strings);

    if (command_list == NULL) {
        return NULL;
    }

    struct many_commands* many_coms = malloc(sizeof(struct many_commands));
    many_coms->commands = command_list;
    many_coms->join_type = join_type;

    // print_many_commands(many_coms);

    return many_coms;
}
