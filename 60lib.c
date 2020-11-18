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
char *fread_string(FILE *f, int endc)
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

void error(char *text, int error_code)
{
    fprintf(stderr, text, error_code);
    exit(error_code);
}

void error_alloc(int error_code)
{
    error("ALLOC FAILED. ERROR CODE %d\n", error_code);
}

void check_alloc(void *x, int error_code)
{
    if (x == NULL)
        error_alloc(error_code);
}

// void free_str(void **x, int arr_len) // y not work? :-'(
void free_str(char **x, int arr_len)
{
    for (int i = 0; i < arr_len; i++)
        free(x[i]);
}

// last error code used by 60lib
#define LAST_60ERR = 1;

#include <string.h>

char *clone_string(char *s)
{
    long size = strlen(s);
    char *clone = malloc(size*sizeof(char));
    check_alloc(clone, 1);

    long i;

    for (i = 0; i < size; i++)
    {
        clone[i] = s[i];
    }

    return clone;
}
