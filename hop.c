#include "headers.h"
char home_dir[MAX_PATH];
char current_dir[MAX_PATH];
char previous_dir[MAX_PATH];
int has_previous;
// Main hop execution function
int execute_hop(char **tokens)
{
    if (tokens[1] == NULL)
    {
        hopToHome();
        return 1;
    }
    for (int i = 1; tokens[i] != NULL; i++)
    {
        if (strcmp(tokens[i], "~") == 0)
        {
            hopToHome();
        }
        else if (strcmp(tokens[i], ".") == 0)
        {
            continue; // DO NOTHING
        }
        else if (strcmp(tokens[i], "..") == 0)
        {
            hopToParent();
        }
        else if (strcmp(tokens[i], "-") == 0)
        {
            hopToPrevious();
        }
        else
        {
            hopToPath(tokens[i]);
        }
    }
    return 1;
}

// Change to home directory
void hopToHome()
{
    char temp_dir[MAX_PATH];
    getcwd(temp_dir, sizeof(temp_dir));
    if (strcmp(current_dir, home_dir) != 0)
    {
        if (chdir(home_dir) != 0)
        {
            // chdir(temp_dir);
            printf("Its an error.Cant change to the home directory\n");
            print_error(home_dir);
            return;
        }
        printf("Successfully changed the directory to home directory\n");
        strcpy(previous_dir, temp_dir);
        has_previous = 1;
        printf("Setting the homedirectory as the current working directory\n");
        if (getcwd(current_dir, sizeof(current_dir)) == NULL)
        {
            printf("Error: Could not update current directory\n");
        }
    }
    return;
}

// Change to parent directory
void hopToParent()
{
    char temp_dir[MAX_PATH];
    getcwd(temp_dir, sizeof(temp_dir));
    if (chdir("..") != 0)
    {
        chdir(temp_dir);
        printf("Its an error .Cant change to the parent directory\n");
        print_error("..");
        return;
    }
    if (getcwd(current_dir, sizeof(current_dir)) != NULL)
    {
        strcpy(previous_dir, temp_dir);
        has_previous = 1;
    }
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        printf("Error: Could not update current directory\n");
    }
    return;
}

void hopToPrevious()
{
    if (!has_previous)
    {
        printf("No previous directory!\n");
        return;
    }
    char temp_dir[MAX_PATH];
    getcwd(temp_dir, sizeof(temp_dir));
    // if (getcwd(temp_dir, sizeof(temp_dir)) == NULL)
    // {
    //     perror("getcwd");
    //     return;
    // }
    if (chdir(previous_dir) != 0)
    {
        print_error(previous_dir);
        return;
    }
    strcpy(previous_dir, temp_dir);
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        printf("Error: Could not update current directory\n");
    }
    printf("Done changing to the previous directory\n");
}

// Change to specified path
void hopToPath(char *path)
{
    char temp_dir[MAX_PATH];
    getcwd(temp_dir, sizeof(temp_dir));
    if (chdir(path) != 0)
    {
        print_error(path);
    }
    strcpy(previous_dir, temp_dir);
    has_previous = 1;
    // Update current directory
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        printf("Error: Could not update current directory\n");
    }
    printf("CHnged the directory to the specified path\n");
    return;
}

// Print error message for directory not found
void print_error(const char *dir)
{
    if (errno == ENOENT)
    {
        printf("No such directory!\n");
    }
    else
    {
        perror("hop");
    }
}
