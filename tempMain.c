#include "headers.h"
char home_dir[MAX_PATH];
char current_dir[MAX_PATH];

void initialize_all_state()
{
    if (getcwd(home_dir, sizeof(home_dir)) == NULL)
    {
        perror("getcwd");
        exit(1);
    }
    // has_previous = 0;
    // initialize_log();
    // No need to call initialize_hop here, as hop.c sets initial state
}

int main()
{
    initialize_all_state();
    while (1)
    {
        char curr_dir_path[MAX_PATH];
        prompt(curr_dir_path, home_dir);

        char *input = takeInputFromUser();
        if (input == NULL)
        {
            printf("\n");
            break;
        }

        if (strlen(input) == 0)
        {
            free(input);
            continue;
        }
        Parser parser;
        int token_count = tokenize(input, &parser);
        if (token_count > 0 && parse_shell_cmd(&parser))
        {
            printf("Your syntax is valid ..yaaayy\n");
            printf("Printed syntax is %s\n", input);
        }
        else if (token_count > 0)
        {
            printf("Invalid Syntax!\n");
        }
        free(input);
    }
    return 0;
}