#ifndef UTILS
#define UTILS

// define constants
#define MAX_INPUT_LENGTH 256
#define MAX_PATH_LENGTH 4096

// define global functions
void do_error(void);

struct command {
    int argc;
    char** argv;
};

// structs and unions to store commands in
struct redirect {  // stores an argv and its redirect
    struct list* commands; // list of command structs, length 1 if > or \0
    char redir_type;  // | or > or \0
    char* out_file; // NULL if | or \0
};

struct many_commands {  // stores all the commands from one line
    struct list* redirects;  // list of redirect structs
    char join_type;  // + or ; or \0 for none
};
struct command* make_command(int argc, char** argv);
void free_command(struct command* cmd);
void print_command(struct command* cmd);
struct redirect* make_redirect(struct list* commands, char redir_type,
        char* out_file);
void free_redirect(struct redirect* redir);
void print_redirect(struct redirect* redir);
struct many_commands* make_many_commands(struct list* commands, char type);
void free_many_commands(struct many_commands* many_com);
void print_many_commands(struct many_commands* many_com);

#endif
