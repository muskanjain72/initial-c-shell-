#include "headers.h"

void execute_sequential_commands(char *input)
{
    char *cmd_start = input;
    char *cmd_end;
    char *cmd_group;

    // Split input by ';'
    while ((cmd_end = strchr(cmd_start, ';')) != NULL)
    {
        // Null-terminate the current command group
        *cmd_end = '\0';
        cmd_group = cmd_start;

        // Check for a background ampersand
        execute_background_or_foreground(cmd_group);

        // Move to the next command group
        cmd_start = cmd_end + 1;
    }

    // Handle the last command group (or the only command group)
    if (strlen(cmd_start) > 0)
    {
        execute_background_or_foreground(cmd_start);
    }
}
