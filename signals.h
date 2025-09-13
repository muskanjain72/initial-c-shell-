#ifndef SIGNALS_H
#define SIGNALS_H

// Track the current foreground process group
// extern pid_t fg_pgid;
// extern char fg_command[256];
// extern Process *head;
// Handler for Ctrl-C (SIGINT)
void sigint_handler(int sig);
// Handler for Ctrl-Z (SIGTSTP)
void sigtstp_handler(int sig);
// Install handlers for SIGINT and SIGTSTP
void init_signal_handlers();

// Handle Ctrl-D (EOF)
void handle_eof();
void fg_wait(pid_t pid, char *cmd_name);
#endif // SIGNALS_H
