#include "headers.h"
extern char home_dir[MAX_PATH];
extern char current_dir[MAX_PATH];
extern char previous_dir[MAX_PATH];
extern int has_previous;
void initialize_all_state()
{
    // this is initializing the hop part
    if (getcwd(home_dir, sizeof(home_dir)) == NULL)
    {
        perror("getcwd");
        exit(1);
    }
    printf("Home directory is %s\n",home_dir);
    has_previous = 0;
    // initialize_log();
    // No need to call initialize_hop here, as hop.c sets initial state
}
int route_command(char **tokens)
{
    if (strcmp(tokens[0], "hop") == 0)
    {
        printf("Its a hop command\n");
        return execute_hop(tokens);
    }
    // else if (strcmp(tokens[0], "reveal") == 0)
    // {
    //     return execute_reveal(tokens);
    // }
    return 0;
}

int main()
{
    initialize_all_state();
    while (1)
    {
        char curr_dir_path[MAX_PATH];
        getcwd(curr_dir_path,sizeof(curr_dir_path));
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
            // printf("Your syntax is valid ..yaaayy\n");
            printf("Printed syntax is %s\n", input);
            route_command(parser.tokens);
            printf("Command executed successfully\n");
        }
        else if (token_count > 0)
        {
            printf("Invalid Syntax!\n");
        }
        free(input);
    }
    return 0;
}