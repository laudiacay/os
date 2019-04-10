#ifndef PARSING
#define PARSING

char* read_until_newline(int batch_mode, FILE* fp);
struct many_commands* parse_input_buffer(char* in_buffer);

#endif
