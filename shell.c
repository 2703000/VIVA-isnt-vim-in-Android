#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

#define MAX_CMD_LENGTH 1024

void execute_ls(const char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("ls");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

void execute_pkg_install(const char *url)
{
    if (url == NULL || url[0] == '\0')
    {
        fprintf(stderr, "pkg install: missing operand\n");
        return;
    }

    char cmd[MAX_CMD_LENGTH];
    snprintf(cmd, MAX_CMD_LENGTH, "curl -O %s", url);

    int status = system(cmd);
    if (status != 0)
    {
        perror("pkg install");
    }
    else
    {
        printf("File downloaded successfully: %s\n", url);
    }
}

int main()
{
    printf("Welcome to use VIVA-shell\n");
    char cwd[MAX_CMD_LENGTH]; // 当前工作目录
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        char cmd[MAX_CMD_LENGTH];
        char *args[MAX_CMD_LENGTH / 2 + 1];
        int should_run = 1;

        while (should_run)
        {
            printf("user @ VIVA %s $ ", cwd);
            fflush(stdout);

            if (!fgets(cmd, MAX_CMD_LENGTH, stdin))
            {
                perror("fgets");
                exit(EXIT_FAILURE);
            }

            // Remove trailing newline
            cmd[strcspn(cmd, "\n")] = '\0';

            // Tokenize the command
            int position = 0;
            args[position] = strtok(cmd, " ");
            while (args[position] != NULL)
            {
                position++;
                args[position] = strtok(NULL, " ");
            }

            if (strcmp(args[0], "clear") == 0)
            {
                system("clear");
            }
            else if (strcmp(args[0], "cd") == 0)
            {
                if (args[1] == NULL)
                {
                    fprintf(stderr, "cd: missing operand\n");
                }
                else
                {
                    if (chdir(args[1]) != 0)
                    {
                        perror("cd");
                    }
                    else
                    {
                        if (getcwd(cwd, sizeof(cwd)) == NULL)
                        {
                            perror("getcwd");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
            else if (strcmp(args[0], "ls") == 0)
            {
                const char *path = ".";
                if (args[1] != NULL)
                {
                    path = args[1];
                }
                execute_ls(path);
            }
            else if (strcmp(args[0], "pkg") == 0 && args[1] != NULL
                     && strcmp(args[1], "install") == 0)
            {
                if (args[2] == NULL)
                {
                    fprintf(stderr, "pkg install: missing operand\n");
                }
                else
                {
                    execute_pkg_install(args[2]);
                }
            }
            else if (strcmp(args[0], "exit") == 0)
            {
                should_run = 0;
            }
            else
            {
                printf("Command not found: %s\n", args[0]);
            }
        }
    }
}