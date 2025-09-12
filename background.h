#ifndef BACKGROUND_H
#define BACKGROUND_H


// Function to handle a single command and determine if it should run in the background
void execute_background_or_foreground(char *cmd_group);

// Function to check and print finished background jobs
void check_and_print_finished_jobs();

#endif // BACKGROUND_H
