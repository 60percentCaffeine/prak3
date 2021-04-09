#include <stdio.h>

int main()
{
    unsigned char n = 0;
    scanf("%hhu", &n);

    unsigned char n2 = (n << 2) | (n >> 6);
    printf("%hhu\n", n2);
    fflush(stdout);

    return 0;
}
