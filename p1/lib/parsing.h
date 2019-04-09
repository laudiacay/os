#ifndef PARSING
#define PARSING

char* read_until_newline(void);
struct many_commands* parse_input_buffer(char* in_buffer);

#endif
