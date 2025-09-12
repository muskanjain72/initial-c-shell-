#ifndef PARSER_H
#define PARSER_H

#define MAX_TOKEN_LEN 256
#define MAX_TOKENS 128
typedef struct
{
    char** tokens; // Array of token strings
    int pos;                                // Current position in token array
    int count;                              // Total number of tokens
} Parser;
int tokenize(char *input, Parser *parser);
int parse_command(char *input,Parser* parser);
int parse_shell_cmd(Parser *p);
int parse_cmd_group(Parser *p);
int parse_atomic(Parser *p);
int parse_name(Parser *p);
int parse_input(Parser *p);
int parse_output(Parser *p);
void free_tokens(Parser *parser);
#endif
