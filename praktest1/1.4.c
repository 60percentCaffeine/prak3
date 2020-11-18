#include <stdlib.h> // NULL, malloc

int *max_of_matrix_lines(int n, int m, int *arr)
{
    int *res = malloc(sizeof(int) * n);
    int i, j;

    // whath
    if (arr == NULL)
        return NULL;
    
    for (i = 1; i <= n; i++)
    {
        int max = *arr;

        for (j = 1; j <= m; j++)
        {
            int idx = i * j - 1;
            int value = *(arr + idx);

            if (value > max)
                max = value;
        }

        *(res + i - 1) = max;
    }

    return res;
}
