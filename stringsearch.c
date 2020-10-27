#include <stdlib.h>
#include <stdio.h>
#include <limits.h> // INT_MAX
#include "60lib.c"

long unsigned int my_strlen(const char* str)
{
    char c = 1;
    int amt;

    if (str == NULL)
        return -1;

    for (amt = 0; c; amt++)
    {
        str++;
        c = *str;
    }

    return amt;
}

// SEGFAULTs on null ptr. that's the default behavior
int my_strncmp(const char* s1, const char* s2, long unsigned int len)
{
    char c1;
    char c2;
    long unsigned int i;

    if (s1 == NULL || s2 == NULL)
        return INT_MAX;

    c1 = *s1;
    c2 = *s2;
 

    for (i = 0; (i < len && (c1 || c2)); i++)
    {
        if (c1 > c2)
            return 1;
        else if (c1 < c2)
            return -1;

        // next char
        c1 = *s1;
        c2 = *s2;
        s1++;
        s2++;
    }

    return 0;
}

int my_strcmp(const char* s1, const char* s2)
{
    return my_strncmp(s1, s2, my_strlen(s1));
}

// first occurence of character c
char* my_strchr(const char* str, int c)
{
    const char* p = str;

    while (*p)
    {
        if (*p == c)
            return (char* )p;

        p++;
    }

    return NULL;
}

// first occurence of string needle
char* my_strstr(const char* haystack, const char* needle)
{
    const char* p = haystack;
    int needle_len; // const

    if (haystack == NULL || needle == NULL)
        return NULL;

    needle_len = my_strlen(needle);

    while ((p = my_strchr(p, *needle)) != 0)
    {
        if (my_strncmp(p, needle, needle_len) == 0)
            return (char* ) p;

        p++;
    }

    return NULL;
}

// 2

void print_str_arr(int len, char **arr)
{
    int i = 0;

    for (i = 0; i < len; i++)
    {
        printf("%s\n", (*arr + i));
    }
}

void search_arr(int arr_len, char **arr, char *needle, int occurences)
{
    int i = 0;

    for (i = 0; i < arr_len; i++)
    {
        char *hay = *arr + i;
        char *ptr = hay;
        int occurs;

        // count occurences
        for (occurs = 0; ((ptr != NULL) && (occurs < occurences)); occurs++)
            ptr = my_strstr(ptr, needle);

        if (occurs >= occurences)
            printf("%d: %s\n", i, hay);
    }
}

// inserts c at the end of *str
/*
void insert_char(char **str, int* len, char c)
{
    (*len)++;
    *str = realloc(*str, *len);
    *(*str + *len - 1) = c;
}

char *read_string()
{
    char c = getchar();
    char *str = NULL;
    int len = 0;

    while (c != '\n' && c != EOF && c)
    {
        insert_char(&str, &len, c); // insert_char increments len for us
        c = getchar();
    }

    insert_char(&str, &len, 0);

    return str;
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
*/

int main()
{
    char **arr = NULL;
    int i;
    int arr_len;
    char *needle = NULL;
    int occurences;

    // ask for array length
    // to know when to stop reading
    arr_len = -1;

    printf("Enter array length: ");
    while (arr_len == -1)
        arr_len = read_uint();

    // read the strings
    for (i = 0; i < arr_len; i++)
    {
        char **ptr;
        arr = realloc(arr, sizeof(arr)*(i + 1));
        ptr = arr + i;
        *ptr = read_string('\n');
    }

    printf("This is the inputted array:\n");
    print_str_arr(arr_len, arr);

    printf("Needle: ");
    needle = read_string('\n');

    printf("Occurences: ");
    occurences = read_uint();

    search_arr(arr_len, arr, needle, occurences);
}
