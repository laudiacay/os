#ifndef UTILS
#define UTILS

// define constants
#define MAX_INPUT_LENGTH 256
#define MAX_PATH_LENGTH 4096

// define global functions
void do_error(void);

// structs and unions to store commands in
struct redirect {  // stores an argv and its redirect
    int argc;
    char** argv;
    char redir_type;  // | or >
    int argc2;
    char** argv2;  // output location or command
};

struct many_commands {  // stores all the commands from one line
    struct list* commands;  // list of redirect structs
    char join_type;  // + or ; or \0 for none
};

struct redirect* make_redirect(int argc, char** argv,
        char redir_type, int argc2, char** argv2);
void free_redirect(struct redirect* redir);
void print_redirect(struct redirect* redir);
struct many_commands* make_many_commands(struct list* commands, char type);
void free_many_commands(struct many_commands* many_com);
void print_many_commands(struct many_commands* many_com);
void free_command_list(struct list* commands);

#endif
