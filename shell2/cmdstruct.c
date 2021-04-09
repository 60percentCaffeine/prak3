#include <stdlib.h>

struct cmd_struct {
    char redirectIn;
    char *in;

    char redirectOut;
    char outAppend;
    char *out;

    char redirectErr;
    char *err;
};

// y no default values?? >:(
struct cmd_struct make_cmd()
{
    struct cmd_struct r;

    r.redirectIn = 0;
    r.in = NULL;

    r.redirectOut = 0;
    r.outAppend = 0;
    r.out = NULL;

    r.redirectErr = 0;
    r.err = NULL;

    return r;
}

void free_cmd(struct cmd_struct r)
{
    if (r.in != NULL)
        free(r.in);

    if (r.out != NULL)
        free(r.out);

    if (r.err != NULL)
        free(r.err);
}
