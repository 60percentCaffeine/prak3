#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define LAST_BIT 0xFF
#include <stdio.h>

// debug stuff
/*
void printBits(size_t const size, void const * const ptr)
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

int mod(int n)
{
    int r = 0;
    int i = 0;

//    printf("mod before: ");
//    printBits(sizeof(n), &n);

    for (i = 0; i < sizeof(int); i++)
    {
        int part = n & LAST_BIT;
        n = n >> 8;
        r = (r << 8) + part;
    }

//    printf("\nmod after: ");
//    printBits(sizeof(r), &r);
//    printf("\n");

    return r;
}

int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDWR);
    if (fd < 0)
        perror("open failed\n");
    
    int buf = -1;
    int bytes_read = read(fd, &buf, sizeof(int));

    while (bytes_read == sizeof(int))
    {
        buf = mod(buf);
        lseek(fd, -sizeof(int), SEEK_CUR);
        if (write(fd, &buf, sizeof(int)) != sizeof(int))
            perror("write failed: disk full?\n");

        bytes_read = read(fd, &buf, sizeof(int));
    }

    close(fd);
    return 0;
}
