#include "headers.h"
void ping_process(pid_t pid, int signal_number)
{
    int actual_signal = signal_number % 32;

    // The system call kill(pid, sig) returns -1 if it fails.
    if (kill(pid, actual_signal) == -1)
    {
        // errno = ESRCH (No such process).
        if (errno == ESRCH)
        {
            printf("No such process found\n");
        }
        else
        {
            // Some other error (e.g., EPERM if not allowed)
            printf("Error sending signal: %s\n", strerror(errno));
        }
        // string.h + strerror(errno) converts them into human-readable messages.
    }
    else
    {
        printf("Sent signal %d to process with pid %d\n",
               signal_number, pid);
    }
}