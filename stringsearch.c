#include <stdlib.h>
#include <stdio.h>
#include <limits.h> // INT_MAX
#include "60lib.c"

// string length
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

// compares first len symbols of s1 and s2
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

// compares s1 and s2
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

// counts needles in the haystack
int str_occurences(char *hay, char *needle)
{
    char *ptr = hay;
    int occurs = -1;

    // count occurences
    while (ptr != NULL)
    {
        occurs++;
        ptr = my_strstr(ptr, needle);

        // move the pointer to prevent an inf loop
        if (ptr != NULL)
            ptr++;
    }

    return occurs;
}

// replace

// shift str to the left by n bytes
void str_shiftl(char *str, int n)
{
    int i;

    for (i = 0; i < my_strlen(str); i++)
        str[i] = str[i + n];
}

// shift str to the right by n bytes
void str_shiftr(char *str, int n)
{
    int i;

    for (i = my_strlen(str) + n; i >= n; i--)
        str[i] = str[i - n];
}

// replace all occurences of needle in *haystack with replace_with,
// allocate more memory if needed.
// requires you to count all occurences of needle beforehand
void replace_str(char **haystack, char *needle, int n_len,
        char *replace_with, int rw_len, int occurences)
{
    // calculate new and old string length
    int str_old_len = my_strlen(*haystack);
    int str_new_len = str_old_len + (rw_len - n_len) * occurences;

    // allocate memory if needed
    if (str_new_len > str_old_len)
    {
        *haystack = realloc(*haystack, str_new_len);
        check_alloc(*haystack, 1);
    }
    // start replacing
    char *ptr_hay = my_strstr(*haystack, needle);
    int hay_i;

    if (ptr_hay == NULL)
        return;

    hay_i = (ptr_hay - *haystack);

    while (1)
    {
        // ptr_hay is a part of the string that starts with needle
        char *ptr_hay = *haystack + hay_i;

        // make room for our replacement. TODO: shift only once
        str_shiftl(ptr_hay, n_len);
        str_shiftr(ptr_hay, rw_len);

        // insert the replacement
        for (int i = 0; i < rw_len; i++)
            ptr_hay[i] = replace_with[i];

        // skip to next occurence
        ptr_hay = my_strstr(*haystack, needle);
        if (ptr_hay == NULL)
            break;

        hay_i = (ptr_hay - *haystack);
    }

    // free unneeded memory
    if (str_new_len < str_old_len)
    {
        *haystack = realloc(*haystack, str_new_len);
        check_alloc(*haystack, 1);
    }
}

// loops thru all strings and does the replacement if they match the criteria
void replace_str_arr(char **arr, int arr_len, char *needle, int n_len,
        char *replace_with, int rw_len, int occurences)
{
    for (int i = 0; i < arr_len; i++)
        if (str_occurences(arr[i], needle) == occurences)
            replace_str(&arr[i], needle, n_len, replace_with, rw_len, occurences);
}

// 2

// what could this possibly do?.. (ʃ⌣́,⌣́ƪ)
void print_str_arr(int len, char **arr)
{
    int i = 0;

    for (i = 0; i < len; i++)
        printf("%s\n", *(arr + i));
}

// searches for strings that have "occurences" occurences of needle and prints them
void search_arr(int arr_len, char **arr, char *needle, int occurences)
{
    int i;

    for (i = 0; i < arr_len; i++)
        if (str_occurences(arr[i], needle) == occurences)
            printf("%d: %s\n", i, arr[i]);
}

int main()
{
    char **arr = NULL;
    int i;
    int arr_len;
    int occurences;
    char *needle = NULL;
    char *replace_with = NULL;

    //
    // INPUT
    //

    // ask for array length
    // to know when to stop reading

    printf("Enter array length: ");
    arr_len = read_uint();
    if (arr_len == -1)
        exit(2);

    printf("Enter the text:\n");
    for (i = 0; i < arr_len; i++)
    {
        char **ptr;
        arr = realloc(arr, sizeof(arr)*(i + 1));
        check_alloc(arr, 1);
        ptr = arr + i;
        *ptr = read_string('\n');
    }

//    printf("This is the inputted array:\n");
//    print_str_arr(arr_len, arr);

    printf("Needle: ");
    needle = read_string('\n');

    printf("Occurences: ");
    occurences = read_uint();

    printf("Replace with: ");
    replace_with = read_string('\n');

    //
    // OUTPUT
    //

    // searches arr for strings with occurences occurences of needle and prints them
    search_arr(arr_len, arr, needle, occurences);

    // silently replaces needle with replace_with for eligible strings
    replace_str_arr(arr, arr_len, needle, my_strlen(needle), replace_with,
            my_strlen(replace_with), occurences);

    // print the changed array
    printf("This is the changed array:\n");
    print_str_arr(arr_len, arr);

    free_str(arr, arr_len);
    free(arr);
    free(needle);
    free(replace_with);
}
