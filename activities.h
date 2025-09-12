#ifndef ACTIVITIES_H
#define ACTIVITIES_H
typedef enum
{
    RUNNING,
    STOPPED
} ProcessState;
typedef struct Process
{
    int job_number;
    pid_t pid;
    char command[256]; // store command name
    ProcessState state;
    struct Process *next;
} Process;
extern Process *head;
void add_process(pid_t pid, const char *cmd);
void remove_process(pid_t pid);
void update_processes();
void list_activities();
void kill_all_processes();
Process* get_job_by_number(int job_number);

#endif