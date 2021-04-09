#include <stdlib.h>

void dynarr_add(char ***arr, int* size, char *s)
{
    *size += 1;
    *arr = realloc(*arr, sizeof(char *) * (*size));
    *(*arr + *size - 1) = s;
}

void dynarr_clear(char ***arr)
{
    free(*arr);
    *arr = NULL;
}

void dynarr_deep_clear(char ***arr, int cnt)
{
    char **s = *arr;

    for (int i = 0; i < cnt; i++)
//    while (*s != NULL)
    {
        free(*s);
        s++;
    }

    dynarr_clear(arr);
}
