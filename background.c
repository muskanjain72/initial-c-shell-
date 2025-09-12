
#include "headers.h"
void execute_background_or_foreground(char *cmd_group)
{
    int is_background = 0;
    char *background_amp = strchr(cmd_group, '&');

    // Remove the '&' and mark as a background job if it exists
    if (background_amp != NULL)
    {
        is_background = 1;
        *background_amp = '\0';
    }

    Parser parser;
    char *temp_input = strdup(cmd_group);

    // Parse the command group
    if (tokenize(temp_input, &parser) > 0)
    {
        route_command(parser.tokens, is_background);
        free_tokens(&parser);
    }

    free(temp_input);
}


void check_and_print_finished_jobs()
{
    int status;
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
    }
}
