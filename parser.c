#include "headers.h"

/*
shell_cmd  ->  cmd_group ((& | ;) cmd_group)* &?
cmd_group ->  atomic (\| atomic)*
atomic -> name (name | input | output)*
input -> < name | <name
output -> > name | >name | >> name | >>name
name -> r"[^|&><;\s]+"
*/
/*int tokenize(char *input, Parser *parser)  // Tokenizes the input string into an array of tokens
{
    parser->pos = 0;
    parser->count = 0;
    parser->tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (parser->tokens == NULL)
    {
        fprintf(stderr, "Memory allocation failed for tokens array.\n");
        return 0;
    }
    char *current = input;
    // printf("Initial Input is %s\n", current);
    while (*current != '\0' && parser->count < MAX_TOKENS)
    {
        // Skip leading whitespace
        while (isspace(*current))
            current++;
        if (*current == '\0')
        {
            break;
        }
        char *token = malloc(MAX_TOKEN_LEN * sizeof(char));
        if (token == NULL)
        {
            fprintf(stderr, "Memory allocation failed for a token.\n");
            // Clean up previously allocated memory
            free_tokens(parser);
            return 0;
        }
        int token_pos = 0;
        if (*current == '>')
        {
            if (*(current + 1) == '>')
            {
                strcpy(token, ">>");
                current += 2;
            }
            else
            {
                strcpy(token, ">");
                current++;
            }
        }
        // Handle single-character tokens (`|`, `&`, `;`, `<`)
        else if (*current == '<')
        {
            strcpy(token, "<");
            current++;
        }
        else if (*current == '|')
        {
            strcpy(token, "|");
            current++;
        }
        else if (*current == '&')
        {
            strcpy(token, "&");
            current++;
        }
        else if (*current == ';')
        {
            strcpy(token, ";");
            current++;
        }
        // Handle regular tokens (names)
        else
        {
            while (*current != '\0' && !isspace(*current) && !strchr("|&;<", *current) && token_pos < MAX_TOKEN_LEN - 1)
            {
                // parser->tokens[parser->count][token_pos++] = *current++;
                token[token_pos++] = *current++;
            }
            // parser->tokens[parser->count][token_pos] = '\0';
            token[token_pos] = '\0';
        }
        // parser->tokens[parser->count] = token;
        // parser->count++;
        if (token_pos > 0 || (token_pos == 0 && strchr("|&><;", token[0])))
        {
            parser->tokens[parser->count] = token;
            parser->count++;
        }
        else
        {
            free(token);
        }
    }
    if (parser->count < MAX_TOKENS)
    {
        parser->tokens[parser->count] = NULL;
    }
    // printf("Total numeber of tokens are %d\n",parser->count);
    // printf("Following r the tokens:\n");
    // for (int i = 0; i < parser->count; i++)
    // {
        // printf("%s\n",parser->tokens[i]);
    // }
    // printf("Done with tokenizing and exiting from function\n");
    return parser->count;
}
*/
/*int tokenize(char *input, Parser *parser) {
    parser->pos = 0;
    parser->count = 0;
    parser->tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (parser->tokens == NULL) {
        fprintf(stderr, "Memory allocation failed for tokens array.\n");
        return 0;
    }

    char *current = input;
    char *token_start;
    bool in_quotes = false;
    
    while (*current != '\0' && parser->count < MAX_TOKENS) {
        // Skip leading whitespace
        while (isspace(*current)) {
            current++;
        }

        if (*current == '\0') {
            break;
        }

        // Handle quoted strings
        if (*current == '"') {
            in_quotes = true;
            current++; // Move past the opening quote
            token_start = current;
            while (*current != '\0' && (*current != '"' || (current > token_start && *(current - 1) == '\\'))) {
                current++;
            }
            *current = '\0'; // Null-terminate the string
            current++; // Move past the closing quote
        }
        // Handle single-character tokens (`|`, `&`, `;`, `<`, `>`)
        else if (strchr("|&;<", *current) || (*current == '>' && *(current + 1) != '>')) {
            token_start = current;
            current++;
        }
        // Handle `>>`
        else if (*current == '>' && *(current + 1) == '>') {
            token_start = current;
            current += 2;
        }
        // Handle regular tokens
        else {
            token_start = current;
            while (*current != '\0' && !isspace(*current) && !strchr("|&;><", *current)) {
                current++;
            }
        }
        
        char *token = strdup(token_start);
        if (token == NULL) {
            fprintf(stderr, "Memory allocation failed for a token.\n");
            free_tokens(parser);
            return 0;
        }

        parser->tokens[parser->count++] = token;
    }

    if (parser->count < MAX_TOKENS) {
        parser->tokens[parser->count] = NULL;
    }

    return parser->count;
}
*/
int tokenize(char *input, Parser *parser)
{
    parser->pos = 0;
    parser->count = 0;
    parser->tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (parser->tokens == NULL) {
        fprintf(stderr, "Memory allocation failed for tokens array.\n");
        return 0;
    }

    char *current = input;
    char *token_start;
    
    while (*current != '\0' && parser->count < MAX_TOKENS) {
        // Skip leading whitespace
        while (isspace(*current)) {
            current++;
        }

        if (*current == '\0') {
            break;
        }

        // Handle quoted strings
        if (*current == '"') {
            current++; // Move past the opening quote
            token_start = current;
            while (*current != '\0' && *current != '"') {
                current++;
            }
            if (*current == '"') {
                *current = '\0'; // Null-terminate the string
                current++; // Move past the closing quote
            }
        }
        // Handle `>>`
        else if (*current == '>' && *(current + 1) == '>') {
            token_start = current;
            current += 2;
        }
        // Handle single-character tokens (`|`, `&`, `;`, `<`, `>`)
        else if (strchr("|&;<>", *current) != NULL) {
            token_start = current;
            current++;
        }
        // Handle regular tokens
        else {
            token_start = current;
            while (*current != '\0' && !isspace(*current) && !strchr("|&;<>", *current)) {
                current++;
            }
        }
        
        // Null-terminate the current token
        char temp_char = *current;
        *current = '\0';
        
        char *token = strdup(token_start);
        if (token == NULL) {
            fprintf(stderr, "Memory allocation failed for a token.\n");
            free_tokens(parser);
            return 0;
        }
        
        *current = temp_char; // Restore the character

        parser->tokens[parser->count++] = token;
    }

    if (parser->count < MAX_TOKENS) {
        parser->tokens[parser->count] = NULL;
    }

    return parser->count;
}

// Check if current token matches expected string
int isMatch(Parser *p, const char *expected)
{
    if (p->pos >= p->count)  // p->pos tells the word to compare
    {
        return 0;
    }
    return strcmp(p->tokens[p->pos], expected) == 0;
}

// Consume current token if it matches expected
int consume_token(Parser *p, const char *expected)
{
    if (isMatch(p, expected))
    {
        p->pos++;
        return 1;
    }
    return 0;
}

// Checks if the current token is a valid name
int isValidName(Parser *p)
{
    if (p->pos >= p->count)
    {
        return 0;
    }
    char *token = p->tokens[p->pos];
    // Check if it's a special token
    // return !(strchr("|&><;", token[0]) || (strlen(token) > 1 && token[0] == '>' && token[1] == '>'));
    // Check if it's a special token
    for (int i = 0; token[i] != '\0'; i++)
    {
        if (strchr("|&><;", token[i]))
            return false;
    }
    return true;
}

// Grammar: name -> r"[^|&><;\s]+"
int parse_name(Parser *p)
{
    if (isValidName(p)) // consume the name token
    {
        p->pos++;
        return 1;
    }
    return 0;
}

// Grammar: input -> < name
int parse_input(Parser *p)
{
    int saved_pos = p->pos;
    if (consume_token(p, "<"))
    {
        if (parse_name(p))  // p->pos has been increemented 
        {
            return 1;
        }
    }

    p->pos = saved_pos;
    return 0;
}

// Grammar: output -> > name | >> name
int parse_output(Parser *p)
{
    int saved_pos = p->pos;

    if (consume_token(p, ">") || consume_token(p, ">>"))
    {
        if (parse_name(p))
        {
            return 1;
        }
    }

    p->pos = saved_pos;
    return 0;
}

// Grammar: atomic -> name (name | input | output)*
int parse_atomic(Parser *p)
{
    // Must start with a name
    if (!parse_name(p))
    {
        return 0;
    }

    // Parse zero or more additional names, inputs, or outputs
    while (p->pos < p->count)
    {
        if (parse_name(p) || parse_input(p) || parse_output(p))
        {
            continue;
        }
        else
        {
            break;
        }
    }

    return 1;
}

// Grammar: cmd_group -> atomic (\| atomic)*
int parse_cmd_group(Parser *p)
{
    // Must start with an atomic
    if (!parse_atomic(p))
    {
        return 0;
    }

    // Parse zero or more "| atomic" sequences
    while (consume_token(p, "|"))
    {
        if (!parse_atomic(p))
        {
            return 0;
        }
    }

    return 1;
}

// Grammar: shell_cmd -> cmd_group ((& | ;) cmd_group)* &?
int parse_shell_cmd(Parser *p)
{
    // Must start with a cmd_group
    if (!parse_cmd_group(p))
    {
        return 0;
    }

    // Parse zero or more "(& | ;) cmd_group" sequences
    while (p->pos < p->count && (isMatch(p, "&") || isMatch(p, ";")))
    {
        char separator = p->tokens[p->pos][0];
        p->pos++; // consume & or ;

        if (separator == '&' && p->pos >= p->count)
        {
            return 1; // Valid: command ends with &
        }

        if (!parse_cmd_group(p))
        {
            return 0;
        }
    }

    // Optional trailing & (already handled in the while loop, this is redundant but harmless)
    if (p->pos < p->count && isMatch(p, "&"))
    {
        p->pos++;
    }

    return 1;
}

// Public function to parse a command
int parse_command(char *input, Parser *parser)
{
    // first Tokenizing the input
    int token_count = tokenize(input, parser);
    if (token_count == 0)
    {
        free_tokens(parser);
        return 1; // Empty input is valid
        
    }
    // printf("Tokens are as follows: \n");
    // for (int i = 0; i < parser->count; i++)
    // {
    //     printf("%s ", parser->tokens[i]);
    // }
    // printf("\n");
    parser->pos = 0;
    int result = parse_shell_cmd(parser);
    // Final check to see if all tokens were consumed
    if (result && (parser->pos == parser->count || (parser->pos == parser->count - 1 && strcmp(parser->tokens[parser->pos], "&") == 0)))
    {
        return 1;
    }
    return 0;
}
void free_tokens(Parser *parser)
{
    // printf("Freeing the tokens\n");
    if (parser && parser->tokens)
    {
        for (int i = 0; i < parser->count; i++)
        {
            if (parser->tokens[i] != NULL)
            {
                free(parser->tokens[i]);
            }
        }
        free(parser->tokens);
        parser->tokens = NULL;
    }
    // printf("DOne with freeing the tokens array\n");
}
