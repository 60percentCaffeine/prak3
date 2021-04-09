#include <stdio.h>
#include <stdlib.h>

void fail()
{
    printf("0\n");
    fflush(stdout);
    exit(1);
}

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) fail();

    int size = -1;
    fscanf(f, "%d", &size);
    if (size < 1) fail();

    double *a = NULL, *b = NULL;
    a = malloc(size*sizeof(double));
    b = malloc(size*sizeof(double));

    if (a == NULL || b == NULL) fail();

    for (int i = 0; i < size; i++)
        if ((fscanf(f, "%lf", (a + i))) != 1) fail();

    for (int i = 0; i < size; i++)
        if ((fscanf(f, "%lf", (b + i))) != 1) fail();

    double res = 0;

    for (int i = 0; i < size; i++)
        res = res + a[i]*b[i];

    printf("%f\n", res);
    fflush(stdout);

    free(a);
    free(b);
    fclose(f);

    return 0;
}
