#include "headers.h"
// Tokenizes the input string into an array of tokens
int tokenize(char *input, Parser *parser)
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

    while (*current != '\0' && parser->count < MAX_TOKENS)
    {
        // Skip leading whitespace
        while (isspace(*current))
            current++;
        if (*current == '\0')
        {
            break;
        }
        // if (*current == '>' && *(current + 1) == '>')
        char *token = malloc(MAX_TOKEN_LEN * sizeof(char));
        if (token == NULL)
        {
            fprintf(stderr, "Memory allocation failed for a token.\n");
            // Clean up already allocated memory
            for (int i = 0; i < parser->count; i++)
            {
                free(parser->tokens[i]);
            }
            free(parser->tokens);
            parser->tokens = NULL;
            return 0;
        }
        int token_pos = 0;
        // if (*current == '>' && (*(current + 1) == '>' || (*(current + 1) == ' ' && *(current + 2) == '>')))

        // {
        //     strcpy(parser->tokens[parser->count], ">>");
        //     current += 2;
        // }
        // else if (*current == '>' && isspace(*(current + 1)))
        // {
        //     char *temp = current + 1;
        //     while (isspace(*temp))
        //     {
        //         temp++;
        //     }
        //     if (*temp == '>')
        //     {
        //         strcpy(parser->tokens[parser->count], ">>");
        //         current = temp + 1;
        //     }
        //     else
        //     {
        //         // It was just a single >
        //         parser->tokens[parser->count][0] = *current;
        //         parser->tokens[parser->count][1] = '\0';
        //         current++;
        //     }
        // }
        // // Handle special single characters (|&;<>)
        // else if (strchr("|&;<>", *current))
        // {
        //     parser->tokens[parser->count][0] = *current;
        //     parser->tokens[parser->count][1] = '\0';
        //     current++;
        // }
        if (*current == '>')
        {
            // Check for the `>>` token
            if (*(current + 1) == '>')
            {
                strcpy(token, ">>");
                current += 2;
            }
            // Check for the `> >` token with a space
            else if (*(current + 1) == ' ' && *(current + 2) == '>')
            {
                strcpy(token, ">>");
                current += 3;
            }
            // Handle single `>` token
            else
            {
                token[0] = *current;
                token[1] = '\0';
                current++;
            }
        }
        // Handle single-character tokens (`|`, `&`, `;`, `<`)
        else if (strchr("|&;<>", *current))
        {
            token[0] = *current;
            token[1] = '\0';
            current++;
        }
        // Handle regular tokens (names)
        else
        {
            // int token_pos = 0;
            while (*current != '\0' && !isspace(*current) && !strchr("|&;<>", *current) && token_pos < MAX_TOKEN_LEN - 1)
            {
                // parser->tokens[parser->count][token_pos++] = *current++;
                token[token_pos++] = *current++;
            }
            // parser->tokens[parser->count][token_pos] = '\0';
            token[token_pos] = '\0';
        }
        parser->tokens[parser->count] = token;
        parser->count++;
    }
    if (parser->count < MAX_TOKENS)
    {
        parser->tokens[parser->count] = NULL;
    }

    return parser->count;
}

// Check if current token matches expected string
int match_token(Parser *p, const char *expected)
{
    if (p->pos >= p->count)
    {
        return 0;
    }
    return strcmp(p->tokens[p->pos], expected) == 0;
}

// Consume current token if it matches expected
int consume_token(Parser *p, const char *expected)
{
    if (match_token(p, expected))
    {
        p->pos++;
        return 1;
    }
    return 0;
}

// Checks if the current token is a valid name
int is_name_token(Parser *p)
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
    if (is_name_token(p)) // consume the name token
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
        if (parse_name(p))
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

    if (consume_token(p, ">") || consume_token(p, ">>") || consume_token(p, "> >"))
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
    while (p->pos < p->count && (match_token(p, "&") || match_token(p, ";")))
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
    if (p->pos < p->count && match_token(p, "&"))
    {
        p->pos++;
    }

    return 1;
}

// Public function to parse a command
int parse_command(char *input, Parser *parser)
{
    // Parser parser;

    // Tokenize input
    int token_count = tokenize(input, parser);
    if (token_count == 0)
    {
        return 1; // Empty input is valid
        if (parser->tokens != NULL)
        {
            free(parser->tokens);
            parser->tokens = NULL;
        }
        return 0;
    }
    printf("Tokens: ");
    for (int i = 0; i < (*parser).count; i++)
    {
        printf("'%s' ", (*parser).tokens[i]);
    }
    printf("\n");
    // Parse the command
    // parser.pos = 0;
    // int result = parse_shell_cmd(&parser);

    // // Success: valid syntax and all tokens consumed
    // return result && parser.pos == parser.count;
    parser->pos = 0;
    int result = parse_shell_cmd(parser);
    if (result && (parser->pos == parser->count || (parser->pos == parser->count - 1 && strcmp(parser->tokens[parser->pos], "&") == 0)))
    {
        return 1;
    }
    return 0;
}
void free_tokens(Parser *parser)
{
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
}
