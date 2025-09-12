#include "headers.h"

void execute_pipeline(char **tokens)
{
    int pipe_fds[2];
    int prev_fd = STDIN_FILENO;
    int cmd_start_index = 0;

    // Store PIDs to wait for all child processes
    pid_t pids[100];
    int pid_count = 0;

    for (int i = 0; tokens[i] != NULL || (i > 0 && strcmp(tokens[i - 1], "|") == 0) || cmd_start_index < i; i++)
    {
        // Check for the end of a command or a pipe
        if (tokens[i] == NULL || strcmp(tokens[i], "|") == 0)
        {
            char **cmd_tokens = &tokens[cmd_start_index];

            // Output redirection must be handled for the last command in the pipeline
            // Check for output redirection
            char *output_file = NULL;
            int append_mode = 0;
            int last_redir_index = -1;
            for (int j = i - 1; j >= cmd_start_index; j--)
            {
                if (strcmp(tokens[j], ">") == 0 || strcmp(tokens[j], ">>") == 0)
                {
                    if (tokens[j + 1] != NULL)
                    {
                        output_file = tokens[j + 1];
                        append_mode = (strcmp(tokens[j], ">>") == 0);
                        last_redir_index = j;
                    }
                    else
                    {
                        fprintf(stderr, "Syntax error: no output file specified after '%s'.\n", tokens[j]);
                        return;
                    }
                    break;
                }
            }
            // Remove redirection tokens from the command arguments passed to execvp
            if (last_redir_index != -1)
            {
                tokens[last_redir_index] = NULL;
            }

            // Create a pipe for the next command if it's not the last one
            if (tokens[i] != NULL)
            {
                if (pipe(pipe_fds) < 0)
                {
                    perror("pipe failed");
                    return;
                }
            }

            pid_t pid = fork();
            if (pid < 0)
            {
                perror("fork failed");
                return;
            }

            if (pid == 0)
            {
                // Child process

                // Set up input for the current command
                if (prev_fd != STDIN_FILENO)
                {
                    dup2(prev_fd, STDIN_FILENO);
                    close(prev_fd);
                }

                // Set up output for the current command
                if (tokens[i] != NULL)
                {
                    // This is not the last command, redirect to the pipe's write end
                    close(pipe_fds[0]); // Close read end in child
                    dup2(pipe_fds[1], STDOUT_FILENO);
                    close(pipe_fds[1]);
                }
                else if (output_file != NULL)
                {
                    // This is the last command and has output redirection
                    int flags = O_WRONLY | O_CREAT;
                    if (append_mode)
                        flags |= O_APPEND;
                    else
                        flags |= O_TRUNC;

                    int fd = open(output_file, flags, 0664);
                    if (fd < 0)
                    {
                        fprintf(stderr, "Unable to create file for writing\n");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                // Check for input redirection
                char *input_file = NULL;
                // int input_redir_index = -1;
                // for (int j = 0; tokens[j] != NULL; j++)
                // {
                //     if (strcmp(tokens[j], "<") == 0)
                //     {
                //         input_file = tokens[j + 1];
                //         // input_redir_index = j;
                //         tokens[j] = NULL; // Terminate command arguments here
                //         break;
                //     }
                // }
                int input_redir_index = -1;
                // Iterate backwards to find the LAST '<' operator
                for (int j = i - 1; j >= cmd_start_index; j--)
                {
                    if (strcmp(tokens[j], "<") == 0)
                    {
                        input_file = tokens[j+1];
                        input_redir_index = j;
                        break; // Found the last one, now stop
                    }
                }

                if (input_file != NULL)
                {
                    int fd = open(input_file, O_RDONLY);
                    if (fd < 0)
                    {
                        fprintf(stderr, "No such file or directory\n");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }

                execvp(cmd_tokens[0], cmd_tokens);
                fprintf(stderr, "Command not found!\n");
                exit(1);
            }
            else
            {
                // Parent process
                pids[pid_count++] = pid;
                if (prev_fd != STDIN_FILENO)
                    close(prev_fd);

                if (tokens[i] != NULL)
                {
                    close(pipe_fds[1]); // Close write end in parent
                    prev_fd = pipe_fds[0];
                }
                // cmd_count = 0;
                cmd_start_index = i + 1;
                if (tokens[i] == NULL)
                    break;
            }
        }
    }
    // else
    // {
    //     cmd_tokens[cmd_count++] = tokens[i];
    // }
    // }

    // Parent waits for all child processes to complete
    for (int i = 0; i < pid_count; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
    return;
    // free(cmd_tokens);
}
