#include <stdlib.h> // NULL

int strlen2(char *s)
{
    int len;

    for (len = 0; ; len++)
    {
        if (*s == 0)
            return len;

        s++;
    }
}

// strncmp - compare s1 to s2 (first amt symbols)
int strncmp2(char *s1, char *s2, int amt)
{
    int i;

    for (i = 0; i < amt; i++)
    {
        if (*(s1 + i) > *(s2 + i))
            return 1;
        else if (*(s1 + i) < *(s2 + i))
            return -1;
    }

    return 0;
}

// strstr - ptr to first occurence of needle
char *strstr2(char *haystack, char *needle)
{
    int i;
    int needle_len = strlen2(needle);

    for (i = 0; ; i++)
    {
        if (strncmp2(haystack, needle, needle_len))
            return haystack;

        haystack++;

        if (*haystack == 0)
            break;
    }

    return NULL;
}

// int NUM - occurences of s2 in s1
int NUM(char *haystack, char *needle)
{
    int occurs;

    // for some reason this is handled diffirently
    if (needle == NULL)
        return 0;

    // invalid input
    if (*needle == 0)
        return -1;

    for (occurs = 0; ; occurs++)
    {
        haystack = strstr2(haystack, needle);

        if (haystack == NULL)
            break;
    }

    return occurs;
}
