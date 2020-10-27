#include <string.h>
#include "60lib.c"

char *newline_to_eof(char *s)
{
    for (; (s != NULL && *s != EOF && *s); s++)
        if (*s == '\n')
        {
            *s = 0;
            return s;
        }

    return s;
}

void put_newline(char *s)
{
    *s = '\n';
}

int main()
{
    char *sFilename;
    char *line;
    FILE *f;

    sFilename = read_string('\n');
    f = fopen(sFilename, "r");

    line = fread_string(f, '\n');

    while (line != NULL && *line != 0)
    {
        // change first \n to EOF and keep its' pointer
        char *newline_ptr = newline_to_eof(line);

        // print the line
        FILE *thread = ((strstr(line, "Copyleft") != NULL) ? stderr : stdout);
        fputs(line, thread);
        fputs("\n", thread);

        // change EOF back to \n
        put_newline(newline_ptr);

        // read next line
        line = fread_string(f, '\n');
    }

    return 0;
}
