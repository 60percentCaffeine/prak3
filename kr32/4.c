#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

volatile char WAITING = 0;
volatile char FINISHED = 0;

void body(int fd[2], int lim, char *STATUS)
{
    int n = -1;

    while (n < lim)
    {
        if (!(n == -1 && strcmp(STATUS, "grandson") == 0))
            read(fd[0], &n, sizeof(int));
        
        printf("%s %d\n", STATUS, lim);
        fflush(stdout);

        n++;
        write(fd[1], &n, sizeof(int));
        if (n >= lim) break;

        WAITING = 1;
        while (WAITING);

        if (FINISHED) break;
    }
}

void sighandl(int s)
{
    WAITING = 0;
    signal(SIGUSR1, sighandl);
}

void sighandl2()
{
    FINISHED = 1;
    WAITING = 0;
    signal(SIGUSR2, sighandl);
}

int main()
{
    signal(SIGUSR1, sighandl);

    int lim = -1;
    scanf("%d", &lim);
    
    int fd[2];

    if (pipe(fd) < 0)
    {
        perror("pipe");
        return 1;
    }

    pid_t pid;

    if ((pid = fork()) < 0)
    {
        perror("fork");
        return 1;
    }
    else if (pid == 0) // son
    {
        pid = fork();
        if (pid == 0) // grandson
        {
            body(fd, lim, "grandson");
            return 0;
        }

        body(fd, lim, "son");
        return 0;
    }
    
    body(fd, lim, "father");
    return 0;
}

