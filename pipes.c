#include "headers.h"
extern void add_process(pid_t pid, const char *cmd,ProcessState state);
void execute_pipeline(char **tokens, int is_background)
{
    int pipe_fds[2];
    /*
    pipe_fds[0]: The read end of the pipe.
    pipe_fds[1]: The write end of the pipe.
    Any data written to pipe_fds[1] can be read from pipe_fds[0]
    */
    int prev_fd = STDIN_FILENO;
    int cmd_start = 0;
    pid_t pids[100];
    int pid_count = 0;

    // for (int i = 0;; i++)
    for (int i = 0; tokens[i] != NULL || (i > 0 && strcmp(tokens[i - 1], "|") == 0) || cmd_start < i; i++)
    {
        if (tokens[i] == NULL || strcmp(tokens[i], "|") == 0)
        {
            char **cmd_tokens = &tokens[cmd_start];
            char *output_file = NULL;
            int append_mode = 0;
            char *input_file = NULL;
            for (int j = i - 1; j >= cmd_start; j--)
            {
                if (strcmp(tokens[j], ">") == 0 || strcmp(tokens[j], ">>") == 0)
                {
                    // if (tokens[j + 1] != NULL)
                    // {
                    //     output_file = tokens[j + 1];
                    //     append_mode = (strcmp(tokens[j], ">>") == 0);
                    //     tokens[j] = NULL;
                    //     break;
                    // }
                    // else
                    // {
                    //     fprintf(stderr, "Syntax error: no output file specified after '%s'.\n", tokens[j]);
                    //     return;
                    // }
                    if (tokens[j + 1] == NULL) {
                        fprintf(stderr, "Syntax error: no output file specified after '%s'.\n", tokens[j]);
                        return;
                    }
                    output_file = tokens[j+1];
                    append_mode = (strcmp(tokens[j], ">>") == 0);
                    tokens[j] = NULL;
                    break;
                }
            }

            for (int j = i - 1; j >= cmd_start; j--)
            {
                if (strcmp(tokens[j], "<") == 0)
                {
                    if (tokens[j + 1] == NULL)
                    {
                        fprintf(stderr, "Syntax error: no input file specified after '<'\n");
                        return;
                    }
                    input_file = tokens[j + 1];
                    tokens[j] = NULL; // Null-terminate the command for execvp
                    break;
                }
            }
            /*for (int j = cmd_start; j < i; j++)
            {
                if (tokens[j] && strcmp(tokens[j], "<") == 0)
                {
                    if (!tokens[j + 1])
                    {
                        fprintf(stderr, "Syntax error: no input file after '<'\n");
                        return;
                    }
                    input_file = tokens[j + 1];
                    tokens[j] = NULL;
                }
                else if (tokens[j] &&
                         (strcmp(tokens[j], ">") == 0 || strcmp(tokens[j], ">>") == 0))
                {
                    if (!tokens[j + 1])
                    {
                        fprintf(stderr, "Syntax error: no output file after '%s'\n", tokens[j]);
                        return;
                    }
                    output_file = tokens[j + 1];
                    append_mode = (strcmp(tokens[j], ">>") == 0);
                    tokens[j] = NULL;
                }
            }
            tokens[i] = NULL;
            char **cmd_tokens = &tokens[cmd_start];
            int pipe_fds[2];
            int need_pipe = (tokens[i] != NULL); // true if not last command
            if (need_pipe && pipe(pipe_fds) < 0)
            {
                perror("pipe");
                return;
            }*/
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
                if (is_background)
                {
                    redirect_stdin_to_null();
                }

                if (input_file != NULL)
                {
                    int fd = open(input_file, O_RDONLY);
                    if (fd < 0)
                    {
                        fprintf(stderr, "No such file or directory: %s\n", strerror(errno));
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
                else if (prev_fd != STDIN_FILENO)
                {
                    dup2(prev_fd, STDIN_FILENO);
                    //"Take the data from the read end of the previous pipe and make it this command's standard input.
                    close(prev_fd);
                }
                if (tokens[i] != NULL)
                {
                    close(pipe_fds[0]); // Close read end in child
                    dup2(pipe_fds[1], STDOUT_FILENO);
                    close(pipe_fds[1]);
                }
                // if (need_pipe)
                // {
                //     close(pipe_fds[0]); // close read end
                //     dup2(pipe_fds[1], STDOUT_FILENO);
                //     close(pipe_fds[1]);
                // }
                else if (output_file != NULL)
                {
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
                execvp(cmd_tokens[0], cmd_tokens);
                fprintf(stderr, "Command not found!\n");
                exit(1);
            }
            else
            {
                pids[pid_count++] = pid;
                if (is_background)
                {
                    add_process(pid, cmd_tokens[0], RUNNING);
                }
                if (prev_fd != STDIN_FILENO)
                    close(prev_fd);

                if (tokens[i] != NULL)
                {
                    close(pipe_fds[1]); // Close write end in parent
                    prev_fd = pipe_fds[0];
                }
                // if (need_pipe)
                // {
                //     close(pipe_fds[1]); // parent keeps read end for next cmd
                //     prev_fd = pipe_fds[0];
                // }
                cmd_start = i + 1;
                if (tokens[i] == NULL)
                    break;
                // if (!need_pipe)
                //     break;
            }
        }
    }
    if (!is_background)
    {
        for (int i = 0; i < pid_count; i++)
        {
            // waitpid(pids[i], NULL, 0);
            fg_wait(pids[i], tokens[cmd_start - 1]);
        }
    }
    return;
}
