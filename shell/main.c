// The Shell, Part One.

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "60lib.c"

#include "list.c"

#define FL_NONE 0 // space
#define FL_END 1 // newline
#define FL_PIPE 2 // |
#define FL_EXIT 3 // EOF

//#define DEBUG

// read 1 argument and set flags
char *read_arg(char *flag)
{
    char c = fgetc(stdin);
    char *str = NULL;
    int len = 0;

    while (c == ' ')
    {
        c = fgetc(stdin);
    }

    while (c != ' ' && c != '\n' && c != '|' && c != EOF && c)
    {
        insert_char(&str, &len, c); // insert_char increments len for us, what a nice function
        c = fgetc(stdin);
    }

    insert_char(&str, &len, 0);

    if (c == EOF)
        *flag = FL_EXIT;
    else if (c == '\n')
        *flag = FL_END;
    else if (c == '|')
        *flag = FL_PIPE;
    else
        *flag = FL_NONE;

    return str;
}

// read the command
void read_cmd(char ***argv, char *flag)
{
    *flag = FL_NONE;
    *argv = NULL;
    int argc = 0;

    while (*flag == FL_NONE)
    {
        char *arg = read_arg(flag);
        if (arg == NULL)
            break;

        // empty string
        if (arg[0] == 0)
        {
            free(arg);
            continue;
        }

        dynarr_add(argv, &argc, arg);
    }
    
    // if *flag != FL_NONE, then we have the full command
}

// execute the command and redirect in/out
int sexec(char **argv, char is_pipe)
{
    if (argv == NULL)
        error("error code %d, command expected\n", 1);

    if (strcmp(argv[0], "exit") == 0)
        exit(0);

    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    // couldn't create the process
    if (pid < 0)
        error("error code %d, fork unsuccessful\n", 2);
    
    // Child
    if (pid == 0)
    {
#ifdef DEBUG
        int debugging = 0;

        while (!debugging)
            sleep(1);
#endif

        // Redirect the command's output to pipe, if needed

        if (is_pipe)
            dup2(fd[1], 1); // 0 - stdin

        close(fd[1]);
        close(fd[0]);

        // Execute the command

        execvp(argv[0], argv);
        error("error code %d, exec unsuccessful\n", 3); // this shouldn't happen... unless there was an error
    }

    //printf("child: %d\n", pid);

    // Parent

    // Use child's output as input
    dup2(fd[0], 0);
    close(fd[1]);
    close(fd[0]);

    // Wait for the child to finish
    int status = 0;
    while (wait(&status) != -1);
    //printf("child status: %d\n", status);

    if (status == 256)
        fprintf(stderr, "60sh: command not found\n");

    return status;
}

int main()
{
    char flag = FL_NONE;
    char **argv = NULL;

    int in_orig = dup(0);

    read_cmd(&argv, &flag);
    while (flag != FL_EXIT)
    {
        sexec(argv, flag == FL_PIPE);
        if (flag == FL_END)
            dup2(in_orig, 0);

        dynarr_deep_clear(&argv);
        read_cmd(&argv, &flag);
    }

    if (argv != NULL)
        dynarr_deep_clear(&argv);
    
    return 0;
}
