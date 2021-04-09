#include <stdio.h>
#include "60lib.h"

#include <unistd.h> // fork
#include <sys/types.h> // pid_t

pid_t fork(); // duplicates the current process
            // returns 0 in child
            // -1 on error

int main()
{
    pid_t pid;
    pid = fork();

    if (pid < 0)
       error(1, "fork");
    else if (pid > 0)
    {
        // execute in parent
    }
    else
    {
        // execute in child
    }

    printf("process pid=%d", pid);
    return 0;
}

// race condition - occurs when exec order is undefined
//
// whether the child or the parent processes get control
// first is undefined -> race cond.

pid_t getppid(); // parent pid
exec(); // runs an executable in the current process

// a
execve(char *path, char *argv[], char *envp[]);
execv(char *path, char *argv[]);
exec(char *path);

execvp(char *file, char *argv[]);
execl(char *path, char *arg1, char *arg1, ..., NULL);
execlp(char *file, char *agrvo, char *argv1, ..., NULL); // searches not only in curdir and root dir, but also in PATH envvar
execle(); // you get the point
// argv: pass filename as first, last should be NULL
// envp: last should be NULL

int main(int argc, char **argv, char **envp)
{
    int a = argv[1];

    if (a == 0)
    {
        execl("/bin/ls", "ls", "-l", NULL);
        printf("error\n"); // this shouldn't happen unless there was an error
    }
    else if (a == 1)
    {
        char *argv[] = {"ls", "-l", NULL};
        execv("/bin/ls", argv);
    }

    return 1;
}

// exits:
void exit(int n); // prepares program for exit (i.e. clears io buffers) and calls _exit
void _exit(int n);
// return n;

// Parent process should be responsible:
// - don't die before his son
// - make sure that it exits gracefully
// - maybe make sure that it did its' job correctly
// etc.

int status; // child exit code
pid_t wait(int *status); // waits for child's death. returns pid of dead child
waitpid(pid, &status, 0); // wait for procecss with PID pid

// zombie - dead, but not yet removed process


// 
// Unnamed (anon) channels
//

int fd[2];
pipe(fd); // creates channel for communication between child and parent

// fd[0] - read descriptor
// fd[1] - write descriptor
// sim. to files, the only difference - no positioning (seek won't work)
// these descriptors should be closed just as file descriptors etc.

int fdd = open("a", 0);
int fdd2 = dup(fdd); // duplicates the file descriptor
dup2(fdd, fdd2); // replace fdd2 with a copy of fdd

// 
// Named channels
//

// > ls | wc -- feed output of ls to wc
// > ls | wc | wc | a.out

int main()
{
    int fd[2];
    pipe(fd);

    if (fork() > 0)
    {
        dup(fd[1], 1); // 1 - stdout
        close(fd[1]);
        close(fd[0]);

        execl("/bin/ls", "ls", NULL);
    }
    else
    {
        dup2(fd[0], 0); // 0 - stdin
        close(fd[1]);
        close(fd[0]);

        execlp("wc", "wc", NULL);
    }

    return 1; // shouldn't happen
} // problem: race condition
