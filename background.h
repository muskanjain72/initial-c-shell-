#ifndef BACKGROUND_H
#define BACKGROUND_H

// Function to handle a single command and determine if it should run in the background
// void execute_background_or_foreground(char *cmd_group);
void execute_command_group(char *cmd_group);
// Function to check and print finished background jobs
void check_and_print_finished_jobs();
void redirect_stdin_to_null();

#endif // BACKGROUND_H
