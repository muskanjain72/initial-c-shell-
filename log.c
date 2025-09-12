
#include "headers.h"
// Do not store any shell_cmd if the command name of an atomic command is log itself.
LogEntry command_log[MAX_LOG_ENTRIES];
int log_count = 0;
int log_start = 0;
char log_file_path[1024];
extern int route_command(char **tokens);
int initialize_log()
{
    char *log_path = get_log_file_path();
    if (!log_path)
    {
        fprintf(stderr, "Error: Could not determine log file path\n");
        return 0;
    }
    strcpy(log_file_path, log_path);
    load_log_from_file();
    return 1;
}

char *get_log_file_path()
{
    static char path[LOG_PATH_SIZE];
    extern char home_dir[MAX_PATH];
    snprintf(path, sizeof(path), "%s/.shell_log", home_dir);
    return path;
}

void add_to_log(char *command)
{
    if (!command || strlen(command) == 0)
        return;

    if (log_count > 0)
    {
        int last_index = (log_start + log_count - 1) % MAX_LOG_ENTRIES;
        if (strcmp(command_log[last_index].command, command) == 0)
            return;
    }
    if (log_count < MAX_LOG_ENTRIES)
    {
        int index = (log_start + log_count) % MAX_LOG_ENTRIES;
        strcpy(command_log[index].command, command);
        log_count++;
    }
    else
    {
        strcpy(command_log[log_start].command, command);
        log_start = (log_start + 1) % MAX_LOG_ENTRIES;
    }
    save_log_to_file();
}

int execute_log(char **tokens)
{
    if (tokens[1] == NULL)
    {
        print_log();
        return 1;
    }
    else if (strcmp(tokens[1], "purge") == 0)
    {
        if (tokens[2] != NULL)
        {
            printf("log: Invalid Syntax!\n");
            return 0;
        }
        purge_log();
        return 1;
    }
    else if (strcmp(tokens[1], "execute") == 0)
    {
        if (tokens[2] == NULL || tokens[3] != NULL)
        {
            printf("log: Invalid Syntax!\n");
            return 0;
        }
        char *endptr;
        long index = strtol(tokens[2], &endptr, 10);
        if (*endptr != '\0' || index <= 0 || index > log_count)
        {
            printf("log: Invalid index!\n");
            return 0;
        }
        char *command_to_execute = get_command_from_log((int)index);
        if (command_to_execute)
        {
            Parser parser;
            if (tokenize(command_to_execute, &parser) > 0 && parse_shell_cmd(&parser))
            {
                printf("Executing: %s\n", command_to_execute);
                printf("Parser tokens:%s\n", parser.tokens);
                printf("Command to execute:%s\n");
                route_command(parser.tokens);
                free_tokens(&parser);
                return 2;
            }
            else
            {
                printf("Invalid command in log: %s\n", command_to_execute);
                return 0;
            }
        }
        return 1;
    }
    else
    {
        printf("log: Invalid Syntax!\n");
        return 0;
    }
}

void print_log()
{
    if (log_count == 0)
        return;

    for (int i = 0; i < log_count; i++)
    {
        int index = (log_start + i) % MAX_LOG_ENTRIES;
        printf("%s\n", command_log[index].command);
    }
}

void purge_log()
{
    log_count = 0;
    log_start = 0;
    save_log_to_file();
}
char *get_command_from_log(int index)
{
    if (log_count == 0 || index < 1 || index > log_count)
    {
        return NULL;
    }
    int actual_index = (log_start + log_count - index) % MAX_LOG_ENTRIES;
    return command_log[actual_index].command;
}

void save_log_to_file()
{
    FILE *file = fopen(log_file_path, "w");
    if (!file)
    {
        fprintf(stderr, "Error opening log file for writing.\n");
        return;
    }
    for (int i = 0; i < log_count; i++)
    {
        int index = (log_start + i) % MAX_LOG_ENTRIES;
        fprintf(file, "%s\n", command_log[index].command);
    }
    fclose(file);
}

void load_log_from_file()
{
    FILE *file = fopen(log_file_path, "r");
    if (!file)
    {
        log_count = 0;
        log_start = 0;
        return;
    }
    char line[MAX_CMD_LENGTH];
    log_count = 0;
    log_start = 0;
    while (fgets(line, sizeof(line), file) && log_count < MAX_LOG_ENTRIES)
    {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0)
        {
            strcpy(command_log[log_count].command, line);
            log_count++;
        }
    }
    fclose(file);
}
