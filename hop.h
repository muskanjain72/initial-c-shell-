#ifndef HOP_H
#define HOP_H
// #include "headers.h"
#define MAX_PATH 1024
// Global variables for directory tracking
extern char current_dir[MAX_PATH];
extern char previous_dir[MAX_PATH];
extern char home_dir[MAX_PATH];
extern int has_previous;
// Function declarations
int execute_hop(char **tokens);
void hopToHome();
void hopToParent();
void hopToPrevious();
void hopToPath(char *path);
void print_error(const char *dir);

#endif
