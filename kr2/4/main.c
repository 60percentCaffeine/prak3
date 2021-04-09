void child();

int modmarker(int x[2], int lim)
{
    int sum = x[0] + x[1];
    x[0] = x[1];
    x[1] = sum;

    return (sum > lim); // send SIGTERM not SIGUSR
}

void sighandl(int s)
{

}

int main()
{
    int x[2] = {0, 1};
    signal(SIGUSR1, &sighandl);

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return 1;
    }

    if (pid == 0)
    {
        child();
    }
}

void child()
{

}
