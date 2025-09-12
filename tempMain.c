#include "headers.h"
extern char home_dir[MAX_PATH];
extern char current_dir[MAX_PATH];
extern char previous_dir[MAX_PATH];
extern int has_previous;
extern LogEntry command_log[MAX_LOG_ENTRIES];
extern int log_count;
extern int log_start;
extern char log_file_path[MAX_PATH];

void initialize_all_state()
{
    // this is initializing the hop part
    if (getcwd(home_dir, sizeof(home_dir)) == NULL)
    {
        perror("getcwd");
        exit(1);
    }
    printf("Home directory is %s\n", home_dir);
    has_previous = 0;
    initialize_log();
    // No need to call initialize_hop here, as hop.c sets initial state
}
void execute_external_command(char **tokens)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        // Forking failed
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    {
        execvp(tokens[0], tokens);

        // execvp only returns if an error occurred.
        // If it reaches here, the command was not found.
        printf("Command not found!\n");
        exit(1);
    }
    else
    {
        // Parent process
        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
    }
}
int route_command(char **tokens)
{
    if (strcmp(tokens[0], "hop") == 0)
    {
        printf("Its a hop command\n");
        return execute_hop(tokens);
    }
    else if (strcmp(tokens[0], "reveal") == 0)
    {
        printf("Its a reveal command\n");
        return execute_reveal(tokens);
    }
    else if (strcmp(tokens[0], "log") == 0)
    {
        printf("Its a log command\n");
        return execute_log(tokens);
    }
    else
    {
        execute_external_command(tokens);
    }
    return 0;
}

int main()
{
    initialize_all_state();
    while (1)
    {
        char curr_dir_path[MAX_PATH];
        getcwd(curr_dir_path, sizeof(curr_dir_path));
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
            bool m = shouldLogCommand(input);
            if (m && !(strcmp(parser.tokens[0], "log") == 0))
                add_to_log(input);
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