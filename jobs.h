#ifndef JOBS_H
#define JOBS_H

// Function prototypes for job control
void execute_fg(char **tokens);
void execute_bg(char **tokens);
int route_jobs_command(char **tokens);

#endif // JOBS_H
