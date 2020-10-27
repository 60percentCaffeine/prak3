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
char *fread_string(FILE *f, char endc)
{
    char c = fgetc(f);
    char *str = NULL;
    int len = 0;

    while (c != endc && c != EOF && c)
    {
        insert_char(&str, &len, c); // insert_char increments len for us, what a nice function
        c = fgetc(f);
    }

    insert_char(&str, &len, 0);

    return str;
}

char *read_string(char endc)
{
    return fread_string(stdin, endc);
}

int read_uint()
{
    char c = getchar();
    int num = 0;

    while (c != '\n')
    {
        if (c < '0' || c > '9')
        {
            printf("ERROR: Enter a positive integer.\n");
            return -1;
        }

        int digit = c - '0';
        num *= 10;
        num += digit;

        c = getchar();
    }

    return num;
}


