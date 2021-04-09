#define _POSIX_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

// returns count
int procarr(int arr[20])
{
    int j = 0;
    for (int i = 0; i < 20; i++)
    {
        if (arr[i] % 2 == 0)
        {
            arr[j] = arr[i];
            j++;
        }
    }

    return j; // count of res
}

volatile int cstop = 0;

void childterm(int s)
{
    if (s == SIGTERM)
    {
        cstop = 1;
    }
}

volatile int cwait = 1; // for parent

void childhandl(int s)
{
    if (s != SIGUSR1)
        return;

    signal(SIGUSR1, childhandl);
    cwait = !cwait;
}

void child(int fd[2])
{
    pid_t ppid = getppid();
    int arr[20]; 
    signal(SIGUSR1, childhandl); 

    while (!cstop)
    {
        while (cwait);

        read(fd[0], arr, sizeof(int)*20);

        int cnt = procarr(arr);
        write(fd[1], &cnt, sizeof(int));
        write(fd[1], arr, sizeof(int)*cnt);

        if (kill(ppid, SIGUSR1) != 0)
        {
            perror("child: can\'t kill\n");
            kill(ppid, SIGKILL);
            exit(2);
        }
    }

    close(fd[0]); close(fd[1]);
}
