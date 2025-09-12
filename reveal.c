#include "headers.h"
/*
int execute_reveal(char **tokens);
int parse_flags(char **tokens, int *show_hidden, int *line_by_line, int *path_index);
int reveal_directory(const char *path, int show_hidden, int line_by_line);
// char* resolve_reveal_path(char *arg);
int compare_strings(const void *a, const void *b);
void printEntries(char **entries, int count, int line_by_line);
*/

int execute_reveal(char** tokens)
{
    int show_hidden = 0;  // -a flag
    int line_by_line = 0; // -l flag
    int path_index = -1;  // Index of path argument
    if (!parse_flags(tokens, &show_hidden, &line_by_line, &path_index))
    {
        printf("reveal: Invalid Syntax!\n");
        return 0;
    }

    const char *target_path = NULL;
    if (path_index == -1)
    {
        // No path argument, default to current directory
        target_path = current_dir;
    }
    else
    {
        const char *arg = tokens[path_index];
        if (strcmp(arg, "~") == 0)
        {
            target_path = home_dir;
        }
        else if (strcmp(arg, "-") == 0)
        {
            if (!has_previous)
            {
                printf("No such directory!\n");
                return 0;
            }
            target_path = previous_dir;
        }
        else if (strcmp(arg, "..") == 0) {
            // Logic for '..'
            char parent_path[MAX_PATH];
            strcpy(parent_path, current_dir);
            char *last_slash = strrchr(parent_path, '/');
            if (last_slash && last_slash != parent_path) {
                *last_slash = '\0';
                target_path = parent_path;
            } else if (last_slash && last_slash == parent_path) {
                // If it's the root directory (e.g., "/")
                target_path = "/";
            } else {
                // No parent directory (e.g., relative path)
                target_path = ".";
            }
        }
        else if (strcmp(arg, ".") == 0)
        {
            // Logic for '.'
            target_path = current_dir;
        }
        else
        {
            target_path = arg;
        }
    }

    // List directory contents
    int result = reveal_directory(target_path, show_hidden, line_by_line);

    return result;
}
int compare_strings(const void *a, const void *b)
{
    char *str1 = *(char **)a;
    char *str2 = *(char **)b;
    return strcmp(str1, str2);
}

// Print entries in either line-by-line or space-separated format
void printEntries(char **entries, int count, int line_by_line)
{
    if (line_by_line)
    {
        for (int i = 0; i < count; i++)
        {
            printf("%s\n", entries[i]);
        }
    }
    else
    {
        for (int i = 0; i < count; i++)
        {
            if (i > 0)
                printf(" ");
            printf("%s", entries[i]);
        }
        if (count > 0)
            printf("\n");
    }
}

// List directory contents
int reveal_directory(const char *path, int show_hidden, int line_by_line)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        printf("No such directory!\n");
        return 0;
    }

    // Read all directory entries
    // Note: The maximum number of entries is fixed at 1000 for simplicity.
    // A more robust solution would dynamically resize the array.
    char **entries = malloc(1000 * sizeof(char *));
    int count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL && count < 1000)
    {
        // Skip hidden files unless -a flag is set
        if (!show_hidden && entry->d_name[0] == '.')
        {
            continue;
        }

        // Skip . and .. entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Allocate and store entry name
        entries[count] = malloc(strlen(entry->d_name) + 1);
        strcpy(entries[count], entry->d_name);
        count++;
    }

    closedir(dir);

    // Sort entries lexicographically using ASCII values
    qsort(entries, count, sizeof(char *), compare_strings);

    printEntries(entries, count, line_by_line);

    // Free all allocated memory for the entries
    for (int i = 0; i < count; i++)
    {
        free(entries[i]);
    }
    free(entries);

    return 1;
}

// Parse reveal flags and arguments
int parse_flags(char **tokens, int *show_hidden, int *line_by_line, int *path_index)
{
    *show_hidden = 0;
    *line_by_line = 0;
    *path_index = -1;
    int path_count = 0;

    for (int i = 1; tokens[i] != NULL; i++)
    {
        if (tokens[i][0] == '-')
        {
            // Process flag
            for (int j = 1; tokens[i][j] != '\0'; j++)
            {
                if (tokens[i][j] == 'a')
                {
                    *show_hidden = 1;
                }
                else if (tokens[i][j] == 'l')
                {
                    *line_by_line = 1;
                }
                else
                {
                    // Invalid flag character
                    return 0;
                }
            }
        }
        else
        {
            // This is a path argument
            if (path_count == 0)
            {
                *path_index = i;
                path_count++;
            }
            else
            {
                // Too many path arguments
                return 0;
            }
        }
    }

    return 1; // Valid syntax
}