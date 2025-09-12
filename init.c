#include "redirect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// Executes an external command with support for input redirection.
void execute_external_command(char **tokens)
{
    char *redirect_file = NULL;
    char *command_args[100];
    int arg_count = 0;
    
    // First, parse tokens to find the redirection operator and filename.
    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strcmp(tokens[i], "<") == 0)
        {
            // Input redirection operator found.
            // The next token must be the filename.
            if (tokens[i + 1] != NULL)
            {
                redirect_file = tokens[i + 1];
                // Stop parsing here. The redirection tokens are not passed to execvp.
                break;
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
        // No command to execute.
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
    }
}/*
#ifndef REDIRECT_H
#define REDIRECT_H

#include <fcntl.h> // Required for open() and file flags

// Function prototype for external command execution with I/O redirection
void execute_external_command(char **tokens);

#endif
*/