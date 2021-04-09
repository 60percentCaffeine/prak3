#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int fd[2];
    pid_t pid;

    if (pipe(fd) < 0)
    {
        perror("pipe");
        return 1;
    }

    if ((pid = fork()) < 0)
    {
        perror("fork");
        return 2;
    }
    else if (pid == 0)
    {
        int var = 3, var2;
        write(fd[1], &var, sizeof(int));
        printf("son send var = %d\n", var);
        read(fd[0], &var2, sizeof(int));
        printf("son received var2 = %d\n", var2);
        close(fd[0]); close(fd[1]);
    }
    else
    {
        int var2, var3 = 5;
        sleep(1);
        read(fd[0], &var2, sizeof(int));
        printf("father received var2 = %d\n", var2);
        write(fd[1], &var3, sizeof(int));
        printf("father send var3 = %d\n", var3);
        close(fd[0]); close(fd[1]);
        wait(NULL);
    }

    return 0;
}
