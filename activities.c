#include "headers.h"

#ifndef WCONTINUED
#define WCONTINUED 0
#endif
static int job_counter = 1;
Process *head = NULL; // linked list head
void add_process(pid_t pid, const char *cmd,ProcessState state)
{
    Process *new_proc = malloc(sizeof(Process));
    if (!new_proc)
    {
        perror("malloc failed");
        return;
    }
    new_proc->pid = pid;
    new_proc->job_number = job_counter++;
    strncpy(new_proc->command, cmd, 255);
    new_proc->command[255] = '\0';
    new_proc->state = state;
    new_proc->next = head;
    head = new_proc;
}
void remove_process(pid_t pid)
{
    Process *curr = head, *prev = NULL;
    while (curr)
    {
        if (curr->pid == pid)
        {
            if (prev)
                prev->next = curr->next;
            else
                head = curr->next;
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}
static int cmp_process(const void *a, const void *b)
{
    Process *pa = *(Process **)a;
    Process *pb = *(Process **)b;
    return strcmp(pa->command, pb->command);
}
void update_processes()
{
    Process *curr = head;
    int status;
    pid_t result;

    while (curr)
    {
        // Non-blocking check of child process
        result = waitpid(curr->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (result > 0)
        {
            if (WIFEXITED(status) || WIFSIGNALED(status))
            {
                // Process ended → remove from list
                printf("background process with pid %d (%s) exited %s\n", curr->pid,
                    curr->command,
                    WIFEXITED(status) ? "normally" : "abnormally");
                pid_t to_remove = curr->pid;
                curr = curr->next;
                remove_process(to_remove);
                continue;
            }
            else if (WIFSTOPPED(status))
            {
                curr->state = STOPPED;
            }
            else if (WIFCONTINUED(status))
            {
                curr->state = RUNNING;
            }
        }
        else
        {
            // Still running → check manually if kill(pid,0) works
            if (kill(curr->pid, 0) == 0)
            {
                // Assume running if not marked stopped
                if (curr->state != STOPPED)
                    curr->state = RUNNING;
            }
            else
            {
                // Process vanished somehow
                printf("background process with pid %d (%s) exited\n", curr->pid, curr->command);
                pid_t to_remove = curr->pid;
                curr = curr->next;
                remove_process(to_remove);
                continue;
            }
        }
        curr = curr->next;
    }
}
void list_activities()
{
    update_processes(); // keep things fresh

    // Count processes
    int count = 0;
    Process *curr = head;
    while (curr)
    {
        count++;
        curr = curr->next;
    }

    if (count == 0)
    {
        printf("No active processes\n");
        return;
    }

    // Collect into array for sorting
    Process **arr = malloc(count * sizeof(Process *));
    if (!arr)
    {
        perror("malloc failed");
        return;
    }

    curr = head;
    for (int i = 0; i < count; i++)
    {
        arr[i] = curr;
        curr = curr->next;
    }
    qsort(arr, count, sizeof(Process *), cmp_process);
    for (int i = 0; i < count; i++)
    {
        printf("[%d] : %s - %s\n",
               arr[i]->pid,
               arr[i]->command,
               arr[i]->state == RUNNING ? "Running" : "Stopped");
    }

    free(arr);
}

void kill_all_processes()
{
    Process *curr = head;
    while (curr)
    {
        kill(curr->pid, SIGKILL);
        curr = curr->next;
    }
}
Process *get_job_by_number(int job_number)
{
    Process *current = head;
    while (current != NULL)
    {
        if (current->job_number == job_number)
        {
            return current; // found!
        }
        current = current->next;
    }
    return NULL; // no such job
}