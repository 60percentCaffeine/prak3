#include <stdio.h>
#include <stdlib.h>

// inserts c at the end of *str
void insert_char(char **str, int* len, char c)
{
    (*len)++;
    *str = realloc(*str, *len);
    *(*str + *len - 1) = c;
}

// just pass 0/EOF as endc if you want to read the entire string/file
char *fread_string(FILE *f, char endc, int lim)
{
    char c = fgetc(f);
    char *str = NULL;
    int len = 0;

    while (c != endc && c != EOF && c)
    {
        if (len == lim) break;

        insert_char(&str, &len, c); // insert_char increments len for us, what a nice function
        c = fgetc(f);
    }

    insert_char(&str, &len, 0);

    return str;
}

char *read_string(char endc, int lim)
{
    return fread_string(stdin, endc, lim);
}

void print_common_symbol(char s[])
{
    int i = 0;
    unsigned char c = 0;
    char accounts[256] = {0};
    int max = 0;
    char cmax = 0;

    for (i = 0; i < 1000; i++)
    {
        c = s[i];
        if (c == 0) break;
        if (c == ' ') continue;
        accounts[c]++;        
    }

    for (i = 0; i < 256; i++)
        if (accounts[i] > max)
        {
            max = accounts[i];
            cmax = i;
        }

    printf("%c %d\n", cmax, max);
    fflush(stdout);
}

int main()
{
    char *s;
    s = read_string('\n', 1000);

    print_common_symbol(s);
    return 0;
}
