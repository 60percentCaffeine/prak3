#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define LAST_BIT 0x01
#include <stdio.h>

// debug stuff
/*void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
}*/

int digits(unsigned int n)
{
    int r = 0;

    for (r = 0; n != 0; r++)
        n = n >> 1;

    return r;
}

unsigned int mod(unsigned int n)
{
    unsigned int r = 0;
    int i = 0;
    int ds = digits(n);

   // printf("mod before: ");
   // printBits(sizeof(n), &n);

    for (i = 0; i < ds; i++)
    {
        int part = (n & LAST_BIT);
        n = n >> 1;
        r = (r << 1) + part;
    }

   // printf("\nmod after: ");
   // printBits(sizeof(r), &r);
   // printf("\n");

    return r;
}

int main(int argc, char **argv)
{
    unsigned int num = 0;
    scanf("%u", &num);
    unsigned int res = mod(num);
    printf("%u\n", res);

    return 0;
}
