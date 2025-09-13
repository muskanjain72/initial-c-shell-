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
    /*char *redirect_file = NULL;
    char *command_args[100];
    int arg_count = 0;

    // First, parse tokens to find the redirection operator and filename.
    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strcmp(tokens[i], "<") == 0)
        {

            if (tokens[i + 1] != NULL)
            {
                redirect_file = tokens[i + 1];
                // Stop parsing here. The redirection tokens are not passed to execvp.
                i++;
            }
            else
            {
                fprintf(stderr, "Syntax error: no input file specified after '<'.\n");
                return;
            }
        }
        else
        {
            command_args[arg_count++] = tokens[i];
        }
    }
    command_args[arg_count] = NULL;

    if (command_args[0] == NULL)
    {
        return;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    {
        // Child process
        if (redirect_file != NULL)
        {
            // Open the file for reading.
            int fd = open(redirect_file, O_RDONLY);
            if (fd < 0)
            {
                // File does not exist or could not be opened.
                fprintf(stderr, "No such file or directory\n");
                exit(1);
            }

            // Redirect standard input (STDIN_FILENO) to the file.
            dup2(fd, STDIN_FILENO);
            close(fd); // Close the original file descriptor.
        }

        // Execute the command with the modified arguments.
        execvp(command_args[0], command_args);

        // execvp only returns if an error occurred.
        fprintf(stderr, "Command not found!\n");
        exit(1);
    }
    else
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }*/
    char *redirect_in  = NULL;  // last input file
    char *redirect_out = NULL;  // last output file
    int   append_out   = 0;     // 0 = truncate (>), 1 = append (>>)
    char *command_args[100];
    int   arg_count = 0;

    // Parse tokens, remembering only the *last* redirection of each kind
    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strcmp(tokens[i], "<") == 0) {
            if (tokens[i+1]) {
                redirect_in = tokens[i+1];
                i++; // skip filename
            } else {
                fprintf(stderr, "Syntax error: no input file specified after '<'.\n");
                return;
            }
        }
        else if (strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">>") == 0) {
            if (tokens[i+1]) {
                redirect_out = tokens[i+1];
                append_out   = (tokens[i][1] == '>'); // 1 if >>
                i++; // skip filename
            } else {
                fprintf(stderr, "Syntax error: no output file specified after '>' or '>>'.\n");
                return;
            }
        }
        else {
            command_args[arg_count++] = tokens[i];
        }
    }
    command_args[arg_count] = NULL;
    if (command_args[0] == NULL) return;   // nothing to run

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    } else if (pid == 0) {
        // ----- Child -----
        // Input redirection
        if (redirect_in) {
            int fd = open(redirect_in, O_RDONLY);
            if (fd < 0) {
                fprintf(stderr, "No such file or directory\n");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        // Output redirection
        if (redirect_out) {
            int flags = O_WRONLY | O_CREAT;
            flags |= append_out ? O_APPEND : O_TRUNC;
            int fd = open(redirect_out, flags, 0644);
            if (fd < 0) {
                fprintf(stderr, "Unable to create file for writing\n");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        execvp(command_args[0], command_args);
        fprintf(stderr, "Command not found!\n");
        exit(1);
    } else {
        // ----- Parent -----
        int status;
        waitpid(pid, &status, 0);
    }
}
int route_command(char **tokens,int is_background)
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
    else if (strcmp(tokens[0], "activities") == 0)
    {
        printf("Its an activities command\n");
        list_activities();
        return 0;
    }
    else
    {
        // execute_external_command(tokens);
        execute_pipeline(tokens);
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
            // printf("Printed syntax is %s\n", input);
            route_command(parser.tokens,0);
            // printf("Command executed successfully\n");
        }
        else if (token_count > 0)
        {
            printf("Invalid Syntax!\n");
        }
        free(input);
    }
    return 0;
}