#include "headers.h"

// Foreground process tracking (declared in signals.h)
pid_t fg_pgid = -1;                 // definition
char fg_command[256] = "";          // definition
pid_t shell_pgid;
// The head of the processes linked list (declared in activities.c)
extern Process *head;

// Handler for Ctrl-C (SIGINT)
void sigint_handler(int sig)
{
    if (fg_pgid > 0)
    {
        // Send SIGINT to the whole foreground process group
        kill(-fg_pgid, SIGINT);
    }
    // Shell itself continues running
}

// Handler for Ctrl-Z (SIGTSTP)
void sigtstp_handler(int sig)
{
    if (fg_pgid > 0)
    {
        // Stop the foreground process group
        kill(-fg_pgid, SIGTSTP);

        // Add stopped process to activities
        // add_process(fg_pgid, fg_command);
        
        // The add_process function adds the new item to the head of the list.
        // We can get the job number from there.
        // if (head != NULL) {
        //     printf("\n[%d] Stopped %s\n", head->job_number, fg_command);
        // }

        // // Reset foreground tracking
        // fg_pgid = -1;
        // fg_command[0] = '\0';
    }
    // Shell itself continues running
}

// Install handlers for SIGINT and SIGTSTP
// void init_signal_handlers()
// {
//     struct sigaction sa_int, sa_tstp;

//     memset(&sa_int, 0, sizeof(sa_int));
//     memset(&sa_tstp, 0, sizeof(sa_tstp));

//     sa_int.sa_handler = sigint_handler;
//     sigemptyset(&sa_int.sa_mask);
//     sa_int.sa_flags = SA_RESTART;

//     sa_tstp.sa_handler = sigtstp_handler;
//     sigemptyset(&sa_tstp.sa_mask);
//     sa_tstp.sa_flags = SA_RESTART;

//     sigaction(SIGINT, &sa_int, NULL);
//     sigaction(SIGTSTP, &sa_tstp, NULL);
// }
void init_signal_handlers()
{
    struct sigaction sa_int, sa_tstp;

    memset(&sa_int, 0, sizeof(sa_int));
    memset(&sa_tstp, 0, sizeof(sa_tstp));

    sa_int.sa_handler = sigint_handler;
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    sa_tstp.sa_handler = sigtstp_handler;
    sa_tstp.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa_tstp, NULL);
}
// Handle Ctrl-D (EOF)
void handle_eof()
{
    printf("logout\n");

    // Kill all child processes tracked in activities
    kill_all_processes();

    exit(0);
}
void fg_wait(pid_t pid, char *cmd_name)
{
    int status;
    fg_pgid = pid;
    strcpy(fg_command, cmd_name);
    
    // The WUNTRACED flag is key: it returns when the process is stopped
    if (waitpid(pid, &status, WUNTRACED) == -1)
    {
        perror("waitpid failed");
    }

    fg_pgid = -1;
    fg_command[0] = '\0';
    
    if (WIFSTOPPED(status))
    {
        printf("\n[%d] Stopped %s\n", pid, cmd_name);
    }
}
