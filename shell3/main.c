// The Shell 2: Electric Bogaloo.

#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "60lib.c"
#include "dict.c"
#include <errno.h>

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

int do_cmd(char **str, int tail);
char CURRENT_CHILD_PID = 0;

char **HISTORY = NULL;
int HISTORY_CNT = 0;

int *TAILS = NULL;
int TAILS_CNT = 0;

// see read_arg
char argbuf[2] = {0, 0};

int buf_fgetc(char **cmdstr, int tail)
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
    {
        c = **cmdstr;
        if (c == 0) return tail;
        (*cmdstr)++;
    }

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

void buf_clear()
{
    buf_eat(1);
    buf_eat(0);
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

char readflag(char c, char **str, int tail)
{
    // read 1 symbol ahead in case it might
    // be ">>" or "2>"
    int c2 = 0;

    if (c == '>' || c == '2')
        c2 = buf_fgetc(str, tail);

    argbuf[0] = c;
    argbuf[1] = c2;

    // getflag sets argbuf
    int fl = getflag(c, c2);
    
    return fl;
}

char *read_arg(char **str, int tail, char *flag, struct cmd_struct *cmd);
void set_redir_dest(struct cmd_struct *cmd, char fl, char *flag, char *dest)
{
    if (fl == FL_INR)
    {
        cmd->redirectIn = 1;
        cmd->in = dest;
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

int line_delimiters[] = {'\n', EOF, 0};

char is_line_delimiter(int c)
{
    return (in_arr(line_delimiters, 3, c));
}

int cmd_delimiters[] = {'\n', '|', EOF, 0};

char is_cmd_delimiter(int c)
{
    return (in_arr(cmd_delimiters, 4, c));
}

char replace_line_delim(char *str, char crep)
{
    for (int i = 0; ; i++)
    {
        char c = str[i];
        if (is_line_delimiter(c))
        {
            str[i] = crep;
            return c;
        }
    }

    return -1;
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

char *read_arg(char **cmdstr, int tail, char *flag, struct cmd_struct *cmd)
{
    int c = buf_fgetc(cmdstr, tail);
    char *str = NULL;
    int len = 0;
    char skip_flag = 0;
    *flag = FL_NONE;

    while (c == ' ')
    {
        c = buf_fgetc(cmdstr, tail);
    }

    //restore2s(&str, &len);

//    while (c != ' ' && c != '\n' && c != '|' && c != EOF && c && c != '<' && c != '>' && c != '2')
    while (!is_arg_delimiter(c))
    {
        if (is_redirect_symbol(c))
        {
            char flag2 = readflag(c, cmdstr, tail);
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
                set_redir_dest(cmd, flag2, flag, read_arg(cmdstr, tail, flag, cmd));

                // redirect acts as an arg delimiter
                skip_flag = 1;
                break;
            }

            // prevent inf. loop bc. of double redir symbols
            buf_eat(0);
        }

        insert_char(&str, &len, c); // insert_char increments len for us, what a nice function
        c = buf_fgetc(cmdstr, tail);
    }

    if (!skip_flag)
        *flag = getflag(c, 0);

    insert_char(&str, &len, 0);
    return str;
}


// read the command
struct cmd_struct read_cmd(char ***argv, int* argc, char *flag, char **str, char tail)
{
    struct cmd_struct cmd = make_cmd();
    *flag = FL_NONE;
    *argv = NULL;
    *argc = 0;

    if (*str == NULL || is_line_delimiter(*(*str)))
    {
        *str = read_string_tail('\n', &tail);

        // add command to history
        dynarr_add(&HISTORY, &HISTORY_CNT, *str);

        // make note of tail
        dynarr_add_int(&TAILS, &TAILS_CNT, tail);

        // add tail to the end of the string
//        int len = strlen(*str) + 2;
//        insert_char(str, &len, 0);
//        *(*str + len - 2) = tail;
    }

    while (*flag == FL_NONE)
    {
        char *arg = read_arg(str, tail, flag, &cmd);
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
        print_error("error code %d, invalid IO redirect\n", 4);
        cmd.failed = 1;
        return cmd;
    }

    if (*flag != FL_EXIT && (*argv)[0] == NULL)
    {
        dynarr_deep_clear(argv, *argc);
        print_error("error code %d, command expected\n", 5);
        cmd.failed = 1;
        return cmd;
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
        //free_cmd(cmd);
        cmd.failed = 1;
        print_error("error code %d, command expected\n", 1);
        return;
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
            if (HISTORY != NULL) dynarr_deep_clear(&HISTORY, HISTORY_CNT);
            free_cmd(cmd);
            error("Error code %d, can't alloc\n", 6);
        }
        getcwd(wd, 100);
        printf("%s\n", wd);
        free(wd);
        return;
    }

    if (strcmp(argv[0], "history") == 0)
    {
        if (argv[1] != NULL)
        {
            int i = string_to_uint(argv[1]);
            if (i < 1 || i > HISTORY_CNT) return;

            char *cmd = HISTORY[i - 1]; // do_cmd can modify the ptr
            do_cmd(&cmd, TAILS[i - 1]);
        }
        else
        {
            for (int i = 0; i < HISTORY_CNT; i++)
            {
                //char end = replace_line_delim(HISTORY[i], 0);
                printf("\t%d\t%s\n", (i + 1), HISTORY[i]);
                //replace_line_delim(HISTORY[i], end);
            }
        }

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
        if (HISTORY != NULL) dynarr_deep_clear(&HISTORY, HISTORY_CNT);
        free_cmd(cmd);
        error("error code %d, fork unsuccessful\n", 2); // "The behavior of shells when fork fails is not specified by POSIX."
        // I assume it can fail due to a lack of available resources (RAM),
        // which will only cause further errors and dilute the logs.
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
        if (HISTORY != NULL) dynarr_deep_clear(&HISTORY, HISTORY_CNT);
        fprintf(stderr, "errno: %d\n", errno);
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

int in_orig = -1;
char flag = FL_NONE;

// quit shell if 0
int do_cmd(char **str, int tail)
{
    char **argv = NULL;
    struct cmd_struct cmd;

    int argc = 0;

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    cmd = read_cmd(&argv, &argc, &flag, str, tail);

    if (cmd.failed)
    {
        *str = NULL;
        buf_clear();
        dup2(in_orig, 0);
        flag = FL_NONE;
        free_cmd(cmd);
        if (argv != NULL)
            dynarr_deep_clear(&argv, argc);
        return 1;
    }

    if (flag != FL_EXIT)
    {
        sexec(argv, argc, cmd, flag == FL_PIPE);
        if (cmd.failed)
        {
            *str = NULL;
            buf_clear();
            dup2(in_orig, 0);
            free_cmd(cmd);
            flag = FL_NONE;
            return 1;
        }

        if (EXITING)
        {
            dynarr_deep_clear(&argv, argc);
            return 0;
        }

        if (flag == FL_END)
            dup2(in_orig, 0);

        // skip the entire command if the process was stopped
        if (STOPPED)
        {
            while (flag == FL_PIPE)
                read_cmd(&argv, &argc, &flag, str, 0);
        }

        dynarr_deep_clear(&argv, argc);

        return 1; // continue
    }

    dynarr_deep_clear(&argv, argc);
    return 0; // quit
}

int main()
{
    in_orig = dup(0);
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    char *str = NULL;

    // call do_cmd() until it returns 0
    while (do_cmd(&str, 0));

    if (HISTORY != NULL)
        dynarr_deep_clear(&HISTORY, HISTORY_CNT);

    if (TAILS != NULL)
        dynarr_clear_int(&TAILS);

    return 0;
}
