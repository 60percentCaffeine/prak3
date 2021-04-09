#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "60lib.c"

#include "list.c"

char **read_command(char *s, int *start, int *argc, char *end)
{
    char **argv = NULL;
    int i;

    if (*end)
        return NULL;

    for (i = *start; s[i] != 0; i++)
    {
        if (s[i] == ' ' || s[i] == '|')
        {
            if (i != *start)
                dynarr_add(&argv, argc, substr(s, *start, i));

            *start = i + 1;

            if (s[i] == '|')
                break;
        }
    }

    if (i != *start)
        dynarr_add(&argv, argc, substr(s, *start, i));

    dynarr_add(&argv, argc, NULL);

    if (s[i] == 0)
        *end = 1;

    return argv;
}

int main()
{
    // read command
    char *s = read_string('\n');
    int start = 0;
    int i = 1;  
   
    // execute the command

    int stdin_orig = dup(0);
    char **argv = NULL;
    int argc = 0;
    char end = 0;

    while ((argv = read_command(s, &start, &argc, &end)) != NULL)
    {
        int fd[2];
        pipe(fd);

        pid_t pid = fork();

        if (pid < 0)
            error("fork, error code %d", 1);
        else if (pid == 0)
        { // child
            //int debugging = 0;

            //while (!debugging)
            //    sleep(1);

            if ((i + 1) != argc)
                dup2(fd[1], 1); // 0 - stdin

            close(fd[1]);
            close(fd[0]);

            execvp(argv[0], argv);
            error("error %d\n", 1); // this shouldn't happen unless there was an error
        }

        printf("child: %d\n", pid);

        dup2(fd[0], 0);
        close(fd[1]);
        close(fd[0]);

        // parent
        int status = 0;
        while (wait(&status) != -1);
        printf("child status: %d\n", status);
        argc = 0;
    }

    dup2(stdin_orig, 0);

    free(argv);
    return 0;
}

