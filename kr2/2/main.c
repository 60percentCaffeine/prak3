#include "child.c"

volatile int pwait = 0;

void sighandl(int s)
{
    if (s != SIGUSR1)
        return;

    pwait = !pwait;
}

int rdsegm(FILE *fr, int a[20])
{
    int red = fread(a, sizeof(int), 20, fr);
    if (red != 20)
        return 1;
    return 0;
}


int main(int argc, char **argv)
{
    int fd[2];

    if (pipe(fd) < 0)
    {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return 1;
    }
    else if (pid == 0)
    {
        child(fd);
        return 0;
    }

    int arr[20];
    signal(SIGUSR1, sighandl); 

    FILE *fr;
    FILE *fw;

    fr = fopen(argv[1], "rb");
    fw = fopen("4res.txt", "w");

    while (1)
    {
        if (rdsegm(fr, arr))
            break;

        write(fd[1], arr, sizeof(int)*20);

        // wait for child
        pwait = 1;
        kill(pid, SIGUSR1);
        while (pwait);

        int cnt = 0;
        read(fd[0], arr, sizeof(int)*cnt);

        int written = fwrite(arr, sizeof(int), cnt, fw);
        if (written != cnt)
        {
            perror("can\'t write");
            kill(pid, SIGKILL);
            exit(2);
        }
    }

    kill(pid, SIGTERM);
    close(fd[0]); close(fd[1]);
    fclose(fr);
    fclose(fw);
}
