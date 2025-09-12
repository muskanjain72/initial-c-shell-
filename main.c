#include "headers.h"
#include "parser.h"
// #include "io.h"
// #include "pipes.h"
// #include "execution.h"
// #include "ping.h"       // Added for the ping command
// #include "activities.h" // Added for the activities command

// extern char home_dir[MAX_PATH];
// extern char current_dir[MAX_PATH];
// extern char previous_dir[MAX_PATH];
// extern int has_previous;
// extern LogEntry command_log[MAX_LOG_ENTRIES];
// extern int log_count;
// extern int log_start;
// extern char log_file_path[MAX_PATH];

// Global forward declarations
void free_tokens(Parser *parser);

void initialize_all_state()
{
    if (getcwd(home_dir, sizeof(home_dir)) == NULL)
    {
        perror("getcwd");
        exit(1);
    }
    has_previous = 0;
    initialize_log();
    // No need to call initialize_hop here, as hop.c sets initial state
}

// This is the primary command router that handles pipes and redirection.
int route_command(char **tokens)
{
    // First, check for a pipe. This has the highest precedence.
    int pipe_index = -1;
    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strcmp(tokens[i], "|") == 0)
        {
            pipe_index = i;
            break;
        }
    }

    if (pipe_index != -1)
    {
        // If a pipe is found, execute the entire pipeline
        return execute_pipeline(tokens);
    }
    else
    {
        // If no pipe, check for I/O redirection
        int in_fd = STDIN_FILENO;
        int out_fd = STDOUT_FILENO;
        int redirection_index = -1;

        for (int i = 0; tokens[i] != NULL; i++)
        {
            if (strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">>") == 0)
            {
                redirection_index = i;
                break;
            }
        }

        // Handle I/O redirection if present
        if (redirection_index != -1)
        {
            // Handle input redirection
            for (int i = 0; i < redirection_index; i++)
            {
                if (strcmp(tokens[i], "<") == 0)
                {
                    char *filename = tokens[i + 1];
                    in_fd = open_input_file(filename);
                    if (in_fd == -1)
                        return 0;
                }
            }

            // Handle output redirection
            for (int i = redirection_index; tokens[i] != NULL; i++)
            {
                if (strcmp(tokens[i], ">") == 0)
                {
                    char *filename = tokens[i + 1];
                    out_fd = open_output_file(filename, 0); // 0 for overwrite
                    if (out_fd == -1)
                        return 0;
                }
                else if (strcmp(tokens[i], ">>") == 0)
                {
                    char *filename = tokens[i + 1];
                    out_fd = open_output_file(filename, 1); // 1 for append
                    if (out_fd == -1)
                        return 0;
                }
            }
            // Null-terminate the command tokens to exclude redirection operators
            tokens[redirection_index] = NULL;
        }

        // Check for built-in commands first, as they don't need a new process
        if (strcmp(tokens[0], "hop") == 0)
        {
            return execute_hop(tokens);
        }
        else if (strcmp(tokens[0], "reveal") == 0)
        {
            return execute_reveal(tokens);
        }
        else if (strcmp(tokens[0], "log") == 0)
        {
            return execute_log(tokens);
        }
        else if (strcmp(tokens[0], "activities") == 0)
        {
            // The syntax is just 'activities', no extra arguments.
            if (tokens[1] != NULL)
            {
                printf("Invalid syntax!\n");
                return 0;
            }
            list_activities();
            return 1;
        }
        else if (strcmp(tokens[0], "ping") == 0)
        {
            // The syntax is 'ping <pid> <signal_number>'
            if (tokens[1] == NULL || tokens[2] == NULL || tokens[3] != NULL)
            {
                printf("Invalid syntax!\n");
                return 0;
            }

            // Convert string arguments to integers
            pid_t pid = (pid_t)strtol(tokens[1], NULL, 10);
            int signal_number = (int)strtol(tokens[2], NULL, 10);

            // Check for invalid number conversion (e.g., if input is not a number)
            if (pid == 0 && strcmp(tokens[1], "0") != 0)
            {
                printf("Invalid syntax!\n");
                return 0;
            }
            if (signal_number == 0 && strcmp(tokens[2], "0") != 0)
            {
                printf("Invalid syntax!\n");
                return 0;
            }

            ping_process(pid, signal_number);
            return 1;
        }
        else
        {
            // For all other commands, execute them in a child process with the right I/O
            return execute_command(tokens, in_fd, out_fd);
        }
    }
}
int main(void)
{
    initialize_all_state();
    while (1)
    {
        char current_dir_path[MAX_PATH];
        if (getcwd(current_dir_path, sizeof(current_dir_path)) == NULL)
        {
            perror("getcwd");
            return 1;
        }

        prompt(current_dir_path, home_dir);

        char *input = takeInput();
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

        // Add to log before parsing, as per requirements
        add_to_log(input);

        if (token_count > 0 && parse_shell_cmd(&parser))
        {
            // Special case for 'log execute'
            if (strcmp(parser.tokens[0], "log") == 0 && parser.count > 1 && strcmp(parser.tokens[1], "execute") == 0)
            {
                char *cmd_from_log = get_command_from_log((int)strtol(parser.tokens[2], NULL, 10));
                if (cmd_from_log)
                {
                    printf("%s\n", cmd_from_log);
                    Parser log_parser;
                    if (tokenize(cmd_from_log, &log_parser) > 0 && parse_shell_cmd(&log_parser))
                    {
                        route_command(log_parser.tokens);
                        free_tokens(&log_parser);
                    }
                    else
                    {
                        fprintf(stderr, "Invalid command in log: %s\n", cmd_from_log);
                    }
                }
            }
            else
            {
                route_command(parser.tokens);
            }
            free_tokens(&parser);
        }
        else if (token_count > 0)
        {
            printf("Invalid Syntax!\n");
        }
        free(input);
    }
    return 0;
}
