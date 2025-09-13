#include "headers.h"
// extern Process *head; // Accessing the linked list head from activities.c

static Process *get_last_job()
{
    // The most recently created job is always the head of the linked list
    return head;
}
int route_jobs_command(char **tokens)
{
    if (strcmp(tokens[0], "fg") == 0)
    {
        execute_fg(tokens);
        return 0;
    }
    else if (strcmp(tokens[0], "bg") == 0)
    {
        execute_bg(tokens);
        return 0;
    }
    return 1; // Not a job control command
}
void execute_fg(char **tokens)
{
    Process *job;
    pid_t pid;

    if (tokens[1] == NULL)
    {
        // No job number specified, get the most recent one
        job = get_last_job();
    }
    else
    {
        // Get job by number
        int job_number = atoi(tokens[1]);
        if (job_number <= 0)
        {
            printf("Invalid syntax!\n");
            return;
        }
        job = get_job_by_number(job_number);
    }

    if (job == NULL)
    {
        printf("No such job\n");
        return;
    }

    pid = job->pid;

    // Print the command that is being brought to foreground
    printf("%s\n", job->command);

    // Set the foreground process group of the terminal to the child's group
    if (tcsetpgrp(STDIN_FILENO, pid) == -1)
    {
        perror("tcsetpgrp failed");
        return;
    }

    // Set foreground tracking variables for signal handlers
    // fg_pgid = pid;
    // strcpy(fg_command, job->command);

    // // Continue the process if it's stopped
    // kill(pid, SIGCONT);
    // Continue the process if it's stopped
    if (job->state == STOPPED)
    {
        kill(pid, SIGCONT);
    }

    // Wait for the job to complete or stop again
    fg_wait(pid, job->command);

    // Give terminal control back to the shell
    if (tcsetpgrp(STDIN_FILENO, shell_pgid) == -1)
    {
        perror("tcsetpgrp failed");
    }

    int status;
    // Wait for the process to complete or stop again
    // if (waitpid(pid, &status, WUNTRACED) == -1)
    // {
    //     perror("waitpid failed");
    //     return;
    // }

    // // Reset the terminal's foreground process group to the shell's group
    // if (tcsetpgrp(STDIN_FILENO, getpgrp()) == -1)
    // {
    //     perror("tcsetpgrp failed");
    // }

    // // Clear foreground tracking variables
    // fg_pgid = -1;
    // fg_command[0] = '\0';
    
    // // Check if the process exited or was stopped
    // if (WIFEXITED(status) || WIFSIGNALED(status))
    // {
    //     // The process exited, remove it from the list
    //     remove_process(pid);
    // }
    // else if (WIFSTOPPED(status))
    // {
    //     // The process was stopped again, update its state
    //     job->state = STOPPED;
    // }
    if (waitpid(pid, &status, WNOHANG) == pid) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            remove_process(pid);
        } else if (WIFSTOPPED(status)) {
            job->state = STOPPED;
        }
    }
}

void execute_bg(char **tokens)
{
    Process *job;
    
    if (tokens[1] == NULL)
    {
        // No job number specified, get the most recent one
        job = get_last_job();
    }
    else
    {
        // Get job by number
        int job_number = atoi(tokens[1]);
        if (job_number <= 0)
        {
            printf("Invalid syntax!\n");
            return;
        }
        job = get_job_by_number(job_number);
    }
    
    if (job == NULL)
    {
        printf("No such job\n");
        return;
    }

    if (job->state == RUNNING)
    {
        printf("Job already running\n");
        return;
    }

    // Resume the job in the background
    kill(job->pid, SIGCONT);
    job->state = RUNNING;

    printf("[%d] %s &\n", job->job_number, job->command);
}
