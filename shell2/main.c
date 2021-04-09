// The Shell 2: Electric Bogaloo.

#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "60lib.c"
#include "dict.c"

#include "list.c"
#include "cmdstruct.c"

#define FL_NONE 0 // space
#define FL_END 1 // newline
#define FL_PIPE 2 // |
#define FL_EXIT 3 // EOF

// these aren't passed to sexec
#define FL_INR 4
#define FL_OUTR 5
#define FL_OUTAPPENDR 6
#define FL_ERRR 7

//#define DEBUG

char CURRENT_CHILD_PID = 0;

// see read_arg
char argbuf[2] = {0, 0};

int buf_fgetc(FILE *f)
{
    int c = 0;

    if (argbuf[1] != 0)
    {
        c = argbuf[0];
        argbuf[0] = argbuf[1];
        argbuf[1] = 0;
    }
    else if (argbuf[0] != 0)
    {
        c = argbuf[0];
        argbuf[0] = 0;
    }
    else
        c = fgetc(f);

    return c;
}

void buf_eat(int i)
{
    // there are 2 chars in buf. we need to remove the 2nd one.
    if (i == 1)
    {
        argbuf[1] = 0;
    }
    else if (i == 0) // there can be 1 or 2 chars in buf, we need to remove the first one.
    {
        argbuf[0] = argbuf[1];
        argbuf[1] = 0;
    }
}

// god I miss dictionaries
char getflag(int c, int c2)
{
    char fl = FL_NONE;

    if (c == EOF)
        fl = FL_EXIT;
    else if (c == '\n')
        fl = FL_END;
    else if (c == '|')
        fl = FL_PIPE;
    else if (c == '<')
        fl = FL_INR;
    else if (c == '>' && c2 == '>')
    {
        fl = FL_OUTAPPENDR;
    }
    else if (c == '>')
        fl = FL_OUTR;
    else if (c == '2' && c2 == '>')
    {
        fl = FL_ERRR;
    }

    return fl;
}

char readflag(char c, char **str, int *len)
{
    // read 1 symbol ahead in case it might
    // be ">>" or "2>"
    int c2 = 0;

    if (c == '>' || c == '2')
        c2 = buf_fgetc(stdin);

    argbuf[0] = c;
    argbuf[1] = c2;

    // getflag sets argbuf
    int fl = getflag(c, c2);
    
    return fl;
}

char *read_arg(char *flag, struct cmd_struct *cmd);
void set_redir_dest(struct cmd_struct *cmd, char fl, char *flag, char *dest)
{
    if (fl == FL_INR)
    {
        cmd->redirectIn = 1;
        cmd->in = dest;

        // spaces
/*        while (*flag == FL_NONE && cmd.in[0] == 0)
        {
            free(cmd.in);
            cmd.in = read_arg(flag);
        }*/
    }
    else if (fl == FL_OUTR || fl == FL_OUTAPPENDR)
    {
        cmd->redirectOut = 1;
        cmd->out = dest;
        cmd->outAppend = (fl == FL_OUTAPPENDR);
    }
    else if (fl == FL_ERRR)
    {
        cmd->redirectErr = 1;
        cmd->err = dest;
    }
}

// read 1 argument and set flags

int cmd_delimiters[] = {'\n', '|', EOF, 0};

char is_cmd_delimiter(int c)
{
    return (in_arr(cmd_delimiters, 4, c));
}

int arg_delimiters[] = {' '};

char is_arg_delimiter(int c)
{
    return (in_arr(arg_delimiters, 1, c) || in_arr(cmd_delimiters, 4, c));
}

int redirect_symbols[] = {'>', '<', '2'};

char is_redirect_symbol(int c)
{
    return (in_arr(redirect_symbols, 3, c));
}

int redirects[] = {FL_INR, FL_OUTR, FL_OUTAPPENDR, FL_ERRR};
#define REDIRECTS_CNT 4

char *read_arg(char *flag, struct cmd_struct *cmd)
{
    int c = buf_fgetc(stdin);
    char *str = NULL;
    int len = 0;
    char skip_flag = 0;
    *flag = FL_NONE;

    while (c == ' ')
    {
        c = buf_fgetc(stdin);
    }

    //restore2s(&str, &len);

//    while (c != ' ' && c != '\n' && c != '|' && c != EOF && c && c != '<' && c != '>' && c != '2')
    while (!is_arg_delimiter(c))
    {
        if (is_redirect_symbol(c))
        {
            char flag2 = readflag(c, &str, &len);
            if (in_arr(redirects, REDIRECTS_CNT, flag2))
            {
                if (flag2 == FL_OUTAPPENDR || flag2 == FL_ERRR) // 2 symbol redirects
                {
                    // eat both symbols from the buffer
                    buf_eat(1);
                    buf_eat(0);
                }
                else // 1 symbol redirect
                    buf_eat(0);

                // set redirect destination
                set_redir_dest(cmd, flag2, flag, read_arg(flag, cmd));

                // redirect acts as an arg delimiter
                skip_flag = 1;
                break;
            }

            // prevent inf. loop bc. of double redir symbols
            buf_eat(0);
        }

        insert_char(&str, &len, c); // insert_char increments len for us, what a nice function
        c = buf_fgetc(stdin);
    }

    if (!skip_flag)
        *flag = getflag(c, 0);

    insert_char(&str, &len, 0);
    return str;
}


// read the command
struct cmd_struct read_cmd(char ***argv, int* argc, char *flag)
{
    struct cmd_struct cmd = make_cmd();
    *flag = FL_NONE;
    *argv = NULL;
    *argc = 0;

    while (*flag == FL_NONE)
    {
        char *arg = read_arg(flag, &cmd);
        if (arg == NULL)
            break;

        // empty string
        if (arg[0] == 0)
        {
            free(arg);
            continue;
        }

        dynarr_add(argv, argc, arg);
    }
    
    dynarr_add(argv, argc, NULL);

    if ((cmd.redirectIn && (cmd.in[0] == 0)) || (cmd.redirectOut && cmd.out[0] == 0) || (cmd.redirectErr && cmd.err[0] == 0))
    {
        if (*argv != NULL) dynarr_deep_clear(argv, *argc);
        free_cmd(cmd);
        error("error code %d, invalid IO redirect\n", 4);
    }

    if (*flag != FL_EXIT && (*argv)[0] == NULL)
    {
        dynarr_deep_clear(argv, *argc);
        free_cmd(cmd);
        error("error code %d, command expected\n", 5);
    }

    // if *flag != FL_NONE, then we have the full command
    return cmd;
}

// SIGTSTP is also called in parent when the child stops
char STOPPED = 0;

void on_parent_stop()
{
    STOPPED = 1;

    kill(CURRENT_CHILD_PID, SIGKILL);
    fprintf(stderr, "PID %d stopped\n", CURRENT_CHILD_PID);
 
    signal(SIGTSTP, SIG_IGN);
}

int wait_for_child(pid_t cpid)
{
    CURRENT_CHILD_PID = cpid;
    int status = 0;
    while (!STOPPED && waitpid(cpid, &status, WUNTRACED) != -1);
    //printf("child status: %d\n", status);

    // ignore SIGSTP again
    signal(SIGTSTP, SIG_IGN);

    //if (status == 256)
    //    fprintf(stderr, "60sh: command error\n");

    CURRENT_CHILD_PID = 0;
    return status;
}

// execute the command and redirect in/out
char EXITING = 0;

void sexec(char **argv, int argc, struct cmd_struct cmd, char is_pipe)
{
    if (argv == NULL)
    {
        free_cmd(cmd);
        error("error code %d, command expected\n", 1);
    }

    // 
    // Built-In Commands
    //

    if (strcmp(argv[0], "exit") == 0)
    {
        EXITING = 1;
        return;
    }

    if (strcmp(argv[0], "cd") == 0)
    {
        chdir(argv[1]);
        return;
    }

    if (strcmp(argv[0], "pwd") == 0)
    {
        char *wd = malloc(128);
        if (wd == NULL)
        {
            dynarr_deep_clear(&argv, argc);
            free_cmd(cmd);
            error("Error code %d, can't alloc\n", 6);
        }
        getcwd(wd, 100);
        printf("%s\n", wd);
        free(wd);
        return;
    }

    int fd[2];
    pipe(fd);

    STOPPED = 0;
    signal(SIGTSTP, on_parent_stop);

    pid_t pid = fork();

    // couldn't create the process
    if (pid < 0)
    {
        dynarr_deep_clear(&argv, argc);
        free_cmd(cmd);
        error("error code %d, fork unsuccessful\n", 2);
    }
    
    // Child
    if (pid == 0)
    {
#ifdef DEBUG
        int debugging = 0;

        while (!debugging)
            sleep(1);
#endif

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        // Redirect the command's output to pipe, if needed
        if (is_pipe)
            dup2(fd[1], 1); // 0 - stdin

        // Redirect STDIN
        if (cmd.redirectIn)
            freopen(cmd.in, "r", stdin);

        // Redirect STDOUT
        if (cmd.redirectOut)
            freopen(cmd.out, (cmd.outAppend ? "a" : "w"), stdout);

        // Redirect STDERR
        if (cmd.redirectErr)
            freopen(cmd.err, "w", stderr);


        close(fd[1]);
        close(fd[0]);

        // Execute the command

        execvp(argv[0], argv);
        if (argv != NULL) dynarr_deep_clear(&argv, argc);
        free_cmd(cmd);
        error("error code %d, exec unsuccessful\n", 3); // this shouldn't happen... unless there was an error
    }

    if (cmd.in) free(cmd.in);
    if (cmd.out) free(cmd.out);
    if (cmd.err) free(cmd.err);

    //printf("child: %d\n", pid);

    // Parent

    // Use child's output as input
    dup2(fd[0], 0);
    close(fd[1]);
    close(fd[0]);

    // Wait for the child to finish
    wait_for_child(pid);
}

int main()
{
    char flag = FL_NONE;
    char **argv = NULL;

    int in_orig = dup(0);
    struct cmd_struct cmd;

    int argc = 0;

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    cmd = read_cmd(&argv, &argc, &flag);
    while (flag != FL_EXIT)
    {
        sexec(argv, argc, cmd, flag == FL_PIPE);
        if (EXITING) break;

        if (flag == FL_END)
            dup2(in_orig, 0);

        // skip the entire command if the process was stopped
        if (STOPPED)
        {
            while (flag == FL_PIPE)
                read_cmd(&argv, &argc, &flag);
        }

        dynarr_deep_clear(&argv, argc);
        cmd = read_cmd(&argv, &argc, &flag);
    }

    if (argv != NULL) dynarr_deep_clear(&argv, argc);
    
    return 0;
}
