
#include "headers.h"
extern int route_command(char **tokens, int is_background);
void execute_command_group(char *cmd_group)
{
    int is_background = 0;
    // char *background_amp = strchr(cmd_group, '&');

    // // Remove the '&' and mark as a background job if it exists
    // if (background_amp != NULL)
    // {
    //     is_background = 1;
    //     *background_amp = '\0';
    // }

    // Parser parser;
    // char *temp_input = strdup(cmd_group);

    // // Parse the command group
    // if (tokenize(temp_input, &parser) > 0 && parse_shell_cmd(&parser))
    // {
    //     route_command(parser.tokens, is_background);
    //     free_tokens(&parser);
    // }
    // Check if the command group ends with '&'
    size_t len = strlen(cmd_group);
    while (len > 0 && (cmd_group[len - 1] == ' ' || cmd_group[len - 1] == '\t'))
    {
        len--;
    }
    if (len > 0 && cmd_group[len - 1] == '&')
    {
        is_background = 1;
        cmd_group[len - 1] = '\0';
    }
    Parser parser;
    char *temp_input = strdup(cmd_group);
    // char **tokens;

    // int token_count = tokenize(temp_input, &parser);
    // if (token_count > 0)
    // {
    //     route_command(parser.tokens, is_background);
    //     free(&parser);
    // }
    if (tokenize(temp_input, &parser) > 0)
    {
        route_command(parser.tokens, is_background);
        // Correctly free the tokens using the dedicated function
        free_tokens(&parser);
    }
    free(temp_input);
}

void redirect_stdin_to_null()
{
    int dev_null_fd = open("/dev/null", O_RDONLY);
    if (dev_null_fd < 0)
    {
        perror("open /dev/null failed");
        return;
    }
    dup2(dev_null_fd, STDIN_FILENO);
    close(dev_null_fd);
}

void check_and_print_finished_jobs()
{
    /*int status;
    pid_t pid;

    // Iterate through the processes and check their status
    Process *current = head;
    while (current != NULL)
    {
        pid = waitpid(current->pid, &status, WNOHANG);

        if (pid == current->pid)
        {
            if (WIFEXITED(status))
            {
                printf("background process with pid %d exited normally\n", current->pid);
            }
            else if (WIFSIGNALED(status))
            {
                printf("background process with pid %d exited abnormally\n", current->pid);
            }

            // Remove the process from the list
            remove_process(pid);

            // Restart iteration after removal, as the list structure may have changed
            current = head;
            continue;
        }

        current = current->next;
    }*/
    update_processes();
}
